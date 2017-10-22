/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#include <numeric>

#include "tensorflow/core/kernels/dataset.h"

#include "tensorflow/core/framework/partial_tensor_shape.h"
#include "tensorflow/core/framework/register_types.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/util/sparse/sparse_tensor.h"

namespace tensorflow {

namespace {

// See documentation in ../ops/iterator_ops.cc for a high-level
// description of the following op.

template <typename T>
class Dataset : public DatasetBase {
 public:
  explicit Dataset(const sparse::SparseTensor& sparse_tensor)
      : sparse_tensor_(sparse_tensor),
        dtypes_({DT_INT64, sparse_tensor.dtype(), DT_INT64}),
        shapes_({{-1, sparse_tensor.dims() - 1},
                 {-1},
                 {sparse_tensor.dims() - 1}}) {}

  std::unique_ptr<IteratorBase> MakeIterator() const override {
    return std::unique_ptr<IteratorBase>(new Iterator(this));
  }

  const DataTypeVector& output_dtypes() const override { return dtypes_; }
  const std::vector<PartialTensorShape>& output_shapes() const override {
    return shapes_;
  }

  string DebugString() override {
    return "SparseTensorSliceDatasetOp::Dataset";
  }

 private:
  class Iterator : public DatasetIterator<Dataset<T>> {
   public:
    explicit Iterator(const Dataset<T>* dataset)
        : DatasetIterator<Dataset<T>>(dataset),
          dataset_(dataset),
          num_elements_(dataset->sparse_tensor_.shape().dim_size(0)),
          dense_shape_(DT_INT64, {dataset->sparse_tensor_.dims() - 1}),
          group_iterable_(dataset->sparse_tensor_.group({0})),
          iter_(group_iterable_.begin()) {
      for (size_t i = 0; i < dense_shape_.NumElements(); ++i) {
        dense_shape_.vec<int64>()(i) =
            dataset_->sparse_tensor_.shape().dim_size(i + 1);
      }
    }

    Status GetNext(IteratorContext* ctx, std::vector<Tensor>* out_tensors,
                   bool* end_of_sequence) override {
      mutex_lock l(mu_);
      if (i_ == num_elements_) {
        *end_of_sequence = true;
        return Status::OK();
      }

      out_tensors->clear();
      out_tensors->reserve(3);
      const int rank = dataset_->sparse_tensor_.dims();

      if (i_ > next_non_empty_i_ && iter_ != group_iterable_.end()) {
        // We still have elements to consume from `group_iterable_`
        // and we have emitted all elements up to and including the
        // current position.
        sparse::Group group = *iter_;
        const auto indices = group.indices();
        const auto values = group.values<T>();
        const int64 num_entries = values.size();
        next_non_empty_i_ = indices(0, 0);

        next_indices_ = Tensor(DT_INT64, {num_entries, rank - 1});
        next_values_ = Tensor(DataTypeToEnum<T>::value, {num_entries});

        auto next_indices_t = next_indices_.matrix<int64>();
        auto next_values_t = next_values_.vec<T>();

        for (int64 i = 0; i < num_entries; ++i) {
          for (int d = 1; d < rank; ++d) {
            next_indices_t(i, d - 1) = indices(i, d);
          }
          next_values_t(i) = values(i);
        }

        ++iter_;
      }

      if (i_ == next_non_empty_i_) {
        // The current position is non-empty in the input
        // `SparseTensor`, and we have already read the value from the
        // `GroupIterable`.
        out_tensors->push_back(std::move(next_indices_));
        out_tensors->push_back(std::move(next_values_));
        out_tensors->push_back(dense_shape_);
        next_non_empty_i_ = kNextNonEmptyUnknown;
      } else {
        DCHECK(i_ < next_non_empty_i_ || iter_ == group_iterable_.end());
        // The current position is empty in the input `SparseTensor`,
        // so emit empty indices and values.
        out_tensors->push_back(Tensor(DT_INT64, TensorShape({0, rank - 1})));
        out_tensors->push_back(Tensor(DataTypeToEnum<T>::value, {0}));
        out_tensors->push_back(dense_shape_);
      }

      ++i_;
      *end_of_sequence = false;
      return Status::OK();
    }

   private:
    const Dataset<T>* const dataset_;
    const int64 num_elements_;

    Tensor dense_shape_;

    mutex mu_;
    sparse::GroupIterable group_iterable_ GUARDED_BY(mu_);
    sparse::GroupIterable::IteratorStep iter_ GUARDED_BY(mu_);
    int64 i_ GUARDED_BY(mu_) = 0;
    const int64 kNextNonEmptyUnknown = -1;
    int64 next_non_empty_i_ GUARDED_BY(mu_) = kNextNonEmptyUnknown;
    Tensor next_indices_ GUARDED_BY(mu_);
    Tensor next_values_ GUARDED_BY(mu_);
  };

  const sparse::SparseTensor sparse_tensor_;
  const DataTypeVector dtypes_;
  const std::vector<PartialTensorShape> shapes_;
};

template <typename T>
class SparseTensorSliceDatasetOp : public OpKernel {
 public:
  explicit SparseTensorSliceDatasetOp(OpKernelConstruction* ctx)
      : OpKernel(ctx) {}

  void Compute(OpKernelContext* ctx) override {
    // Create a new SparseTensorSliceDatasetOp::Dataset, insert it in
    // the step container, and return it as the output.
    const Tensor* indices;
    OP_REQUIRES_OK(ctx, ctx->input("indices", &indices));
    const Tensor* values;
    OP_REQUIRES_OK(ctx, ctx->input("values", &values));
    const Tensor* dense_shape;
    OP_REQUIRES_OK(ctx, ctx->input("dense_shape", &dense_shape));

    OP_REQUIRES(ctx, TensorShapeUtils::IsMatrix(indices->shape()),
                errors::InvalidArgument(
                    "Input indices should be a matrix but received shape ",
                    indices->shape().DebugString()));
    OP_REQUIRES(ctx, TensorShapeUtils::IsVector(values->shape()),
                errors::InvalidArgument(
                    "Input values should be a vector but received shape ",
                    indices->shape().DebugString()));
    OP_REQUIRES(ctx, TensorShapeUtils::IsVector(dense_shape->shape()),
                errors::InvalidArgument(
                    "Input shape should be a vector but received shape ",
                    dense_shape->shape().DebugString()));

    // We currently ensure that `sparse_tensor` is ordered in the
    // batch dimension.
    // TODO(mrry): Investigate ways to avoid this unconditional check
    // if we can be sure that the sparse tensor was produced in an
    // appropriate order (e.g. by `tf.parse_example()` or a Dataset
    // that batches elements into rows of a SparseTensor).
    int64 previous_batch_index = -1;
    for (int64 i = 0; i < indices->dim_size(0); ++i) {
      int64 next_batch_index = indices->matrix<int64>()(i, 0);
      OP_REQUIRES(
          ctx, next_batch_index >= previous_batch_index,
          errors::Unimplemented("The SparseTensor must be ordered in the batch "
                                "dimension; handling arbitrarily ordered input "
                                "is not currently supported."));
      previous_batch_index = next_batch_index;
    }
    gtl::InlinedVector<int64, 8> std_order(dense_shape->NumElements(), 0);
    sparse::SparseTensor sparse_tensor(
        *indices, *values, TensorShape(dense_shape->vec<int64>()), std_order);

    DatasetBase* dataset = new Dataset<T>(sparse_tensor);
    Tensor* output = nullptr;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({}), &output));
    ResourceHandle handle = MakeResourceHandle<DatasetBase>(
        ctx, ctx->step_container()->name(), name());
    OP_REQUIRES_OK(ctx, CreateResource(ctx, handle, dataset));
    output->flat<ResourceHandle>()(0) = handle;
  }

 private:
};

#define REGISTER_DATASET_KERNEL(type)                           \
  REGISTER_KERNEL_BUILDER(Name("SparseTensorSliceDataset")      \
                              .Device(DEVICE_CPU)               \
                              .TypeConstraint<type>("Tvalues"), \
                          SparseTensorSliceDatasetOp<type>);

TF_CALL_ALL_TYPES(REGISTER_DATASET_KERNEL);
#undef REGISTER_DATASET_KERNEL

}  // namespace

}  // namespace tensorflow
