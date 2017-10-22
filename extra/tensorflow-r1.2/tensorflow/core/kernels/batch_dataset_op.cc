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
#include "tensorflow/core/kernels/dataset.h"

#include "tensorflow/core/framework/partial_tensor_shape.h"
#include "tensorflow/core/framework/tensor.h"

namespace tensorflow {

namespace {

// See documentation in ../ops/iterator_ops.cc for a high-level
// description of the following op.

class BatchDatasetOp : public OpKernel {
 public:
  explicit BatchDatasetOp(OpKernelConstruction* ctx) : OpKernel(ctx) {}

  void Compute(OpKernelContext* ctx) override {
    // Create a new BatchDatasetOp::Dataset, insert it in the step-local
    // container, and return it as the output.
    DatasetBase* input;
    OP_REQUIRES_OK(ctx, LookupResource(ctx, HandleFromInput(ctx, 0), &input));
    core::ScopedUnref unref_input(input);

    const Tensor* batch_size_t;
    OP_REQUIRES_OK(ctx, ctx->input("batch_size", &batch_size_t));
    OP_REQUIRES(ctx, TensorShapeUtils::IsScalar(batch_size_t->shape()),
                errors::InvalidArgument("batch_size must be a scalar"));
    const int64 batch_size = batch_size_t->flat<int64>()(0);
    OP_REQUIRES(
        ctx, batch_size > 0,
        errors::InvalidArgument("Batch size must be greater than zero."));

    DatasetBase* dataset = new Dataset(batch_size, input);
    Tensor* output = nullptr;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({}), &output));
    ResourceHandle handle = MakeResourceHandle<DatasetBase>(
        ctx, ctx->step_container()->name(), name());
    OP_REQUIRES_OK(ctx, CreateResource(ctx, handle, dataset));
    output->flat<ResourceHandle>()(0) = handle;
  }

 private:
  class Dataset : public DatasetBase {
   public:
    Dataset(int64 batch_size, const DatasetBase* input)
        : batch_size_(batch_size), input_(input) {
      input_->Ref();

      // NOTE(mrry): Currently we implement "batch up to" semantics. If
      // we could tell statically that the input dataset is infinite,
      // then we could always report `batch_size` as the 0th dimension.
      const auto& input_shapes = input_->output_shapes();
      output_shapes_.reserve(input_shapes.size());
      for (const auto& input_shape : input_shapes) {
        output_shapes_.emplace_back(
            PartialTensorShape({-1}).Concatenate(input_shape));
      }
    }

    ~Dataset() override { input_->Unref(); }

    std::unique_ptr<IteratorBase> MakeIterator() const override {
      return std::unique_ptr<IteratorBase>(new Iterator(this));
    }

    const DataTypeVector& output_dtypes() const override {
      return input_->output_dtypes();
    }

    const std::vector<PartialTensorShape>& output_shapes() const override {
      return output_shapes_;
    }

    string DebugString() override {
      return strings::StrCat("BatchDatasetOp(", batch_size_, ")::Dataset");
    }

   private:
    // Copies element into the index^th slice of parent (in the 0th dimension).
    //
    // TODO(mrry): Reconcile this method with the similar method in
    // the queue implementation.
    template <DataType DT>
    static Status HandleElementToSlice(const Tensor& element, Tensor* parent,
                                       int index) {
      typedef typename EnumToDataType<DT>::Type T;
      if (element.NumElements() !=
          (parent->NumElements() / parent->dim_size(0))) {
        TensorShape chip_shape = parent->shape();
        chip_shape.RemoveDim(0);
        return errors::Internal(
            "HandleElementToSlice Cannot copy slice: number of elements does "
            "not "
            "match.  Shapes are: [element]: ",
            element.shape().DebugString(),
            ", [parent slice]: ", chip_shape.DebugString());
      }
      auto parent_as_matrix = parent->flat_outer_dims<T>();
      parent_as_matrix.chip(index, 0) = element.flat<T>();
      return Status::OK();
    }

    // Copies element into the index^th slice of parent (in the 0th dimension).
    static Status CopyElementToSlice(const Tensor& element, Tensor* parent,
                                     int64 index) {
#define HANDLE_TYPE(DT)                                                   \
  if (element.dtype() == DT) {                                            \
    TF_RETURN_IF_ERROR(HandleElementToSlice<DT>(element, parent, index)); \
    return Status::OK();                                                  \
  }
      HANDLE_TYPE(DT_FLOAT);
      HANDLE_TYPE(DT_HALF);
      HANDLE_TYPE(DT_DOUBLE);
      HANDLE_TYPE(DT_INT32);
      HANDLE_TYPE(DT_UINT8);
      HANDLE_TYPE(DT_INT16);
      HANDLE_TYPE(DT_INT8);
      HANDLE_TYPE(DT_STRING);
      HANDLE_TYPE(DT_COMPLEX64);
      HANDLE_TYPE(DT_COMPLEX128);
      HANDLE_TYPE(DT_INT64);
      HANDLE_TYPE(DT_BOOL);
      HANDLE_TYPE(DT_QINT8);
      HANDLE_TYPE(DT_QUINT8);
      HANDLE_TYPE(DT_QINT32);
      HANDLE_TYPE(DT_QINT16);
      HANDLE_TYPE(DT_QUINT16);
#undef HANDLE_TYPE
      return errors::Unimplemented("CopyElementToSlice Unhandled data type: ",
                                   element.dtype());
    }

    class Iterator : public DatasetIterator<Dataset> {
     public:
      explicit Iterator(const Dataset* dataset)
          : DatasetIterator<Dataset>(dataset),
            input_impl_(dataset->input_->MakeIterator()) {}

      Status GetNext(IteratorContext* ctx, std::vector<Tensor>* out_tensors,
                     bool* end_of_sequence) override {
        // Each row of `batch_elements` is a tuple of tensors from the
        // input iterator.
        std::vector<std::vector<Tensor>> batch_elements;
        batch_elements.reserve(dataset()->batch_size_);
        {
          mutex_lock l(mu_);
          *end_of_sequence = false;
          for (int i = 0; i < dataset()->batch_size_ && !*end_of_sequence;
               ++i) {
            std::vector<Tensor> batch_element_tuple;
            TF_RETURN_IF_ERROR(input_impl_->GetNext(ctx, &batch_element_tuple,
                                                    end_of_sequence));
            if (!*end_of_sequence) {
              batch_elements.emplace_back(std::move(batch_element_tuple));
            }
          }
        }

        if (batch_elements.empty()) {
          DCHECK(*end_of_sequence);
          return Status::OK();
        }

        // Copy the retrieved batch elements into one output tensor
        // per tuple component.
        // NOTE(mrry): If the input or output sizes are statically
        // known, we could potentially read the input values in-place
        // into their respective slice locations. This would require a
        // different GetNext() overload that supports zero-copy, and might
        // make sense in an optimization pass.
        const size_t num_tuple_components = batch_elements[0].size();
        const int64 num_batch_elements = batch_elements.size();
        for (size_t component_index = 0; component_index < num_tuple_components;
             ++component_index) {
          const Tensor& first_element = batch_elements[0][component_index];
          TensorShape batch_component_shape({num_batch_elements});
          batch_component_shape.AppendShape(first_element.shape());
          Tensor batch_component(cpu_allocator(), first_element.dtype(),
                                 batch_component_shape);
          // Build the output tuple component by copying one slice
          // from each input element in the batch.
          for (size_t i = 0; i < num_batch_elements; ++i) {
            TF_RETURN_IF_ERROR(CopyElementToSlice(
                batch_elements[i][component_index], &batch_component, i));
          }
          out_tensors->emplace_back(std::move(batch_component));
        }
        *end_of_sequence = false;
        return Status::OK();
      }

     private:
      mutex mu_;
      int64 i_ GUARDED_BY(mu_);
      std::unique_ptr<IteratorBase> input_impl_ GUARDED_BY(mu_);
    };

    const int64 batch_size_;
    const DatasetBase* const input_;
    std::vector<PartialTensorShape> output_shapes_;
  };
};

REGISTER_KERNEL_BUILDER(Name("BatchDataset").Device(DEVICE_CPU),
                        BatchDatasetOp);

}  // namespace

}  // namespace tensorflow
