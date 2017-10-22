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

#include "tensorflow/core/common_runtime/function.h"
#include "tensorflow/core/framework/partial_tensor_shape.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/lib/random/random.h"

#include "tensorflow/core/kernels/captured_function.h"

namespace tensorflow {

namespace {

// See documentation in ../ops/iterator_ops.cc for a high-level
// description of the following op.

class FilterDatasetOp : public OpKernel {
 public:
  explicit FilterDatasetOp(OpKernelConstruction* ctx)
      : OpKernel(ctx), graph_def_version_(ctx->graph_def_version()) {
    OP_REQUIRES_OK(ctx, ctx->GetAttr("predicate", &func_));
  }

  void Compute(OpKernelContext* ctx) override {
    DatasetBase* input;
    OP_REQUIRES_OK(ctx, LookupResource(ctx, HandleFromInput(ctx, 0), &input));
    core::ScopedUnref unref_input(input);

    OpInputList inputs;
    OP_REQUIRES_OK(ctx, ctx->input_list("other_arguments", &inputs));
    std::vector<Tensor> other_arguments;
    other_arguments.reserve(inputs.size());
    for (const Tensor& t : inputs) {
      other_arguments.push_back(t);
    }

    std::unique_ptr<CapturedFunction> captured_func;
    OP_REQUIRES_OK(ctx, CapturedFunction::Create(ctx, func_, graph_def_version_,
                                                 std::move(other_arguments),
                                                 &captured_func));

    DatasetBase* dataset = new Dataset(input, std::move(captured_func));

    Tensor* output = nullptr;
    OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({}), &output));
    ResourceHandle handle = MakeResourceHandle<DatasetBase>(
        ctx, ctx->step_container()->name(), name());
    OP_REQUIRES_OK(ctx, CreateResource(ctx, handle, dataset));
    output->flat<ResourceHandle>()(0) = handle;
  }

 private:
  const int graph_def_version_;

  class Dataset : public DatasetBase {
   public:
    Dataset(const DatasetBase* input,
            std::unique_ptr<CapturedFunction> captured_func)
        : input_(input), captured_func_(std::move(captured_func)) {
      input_->Ref();
    }

    ~Dataset() override { input_->Unref(); }

    std::unique_ptr<IteratorBase> MakeIterator() const override {
      return std::unique_ptr<IteratorBase>(new Iterator(this));
    }

    const DataTypeVector& output_dtypes() const override {
      return input_->output_dtypes();
    }
    const std::vector<PartialTensorShape>& output_shapes() const override {
      return input_->output_shapes();
    }

    string DebugString() override { return "FilterDatasetOp::Dataset"; }

   private:
    class Iterator : public DatasetIterator<Dataset> {
     public:
      explicit Iterator(const Dataset* dataset)
          : DatasetIterator<Dataset>(dataset),
            input_impl_(dataset->input_->MakeIterator()) {}

      Status GetNext(IteratorContext* ctx, std::vector<Tensor>* out_tensors,
                     bool* end_of_sequence) override {
        // NOTE(mrry): This method is thread-safe as long as
        // `input_impl_` and `f` are thread-safe. However, if multiple
        // threads enter this method, outputs may be observed in a
        // non-deterministic order.
        bool matched;
        do {
          TF_RETURN_IF_ERROR(
              input_impl_->GetNext(ctx, out_tensors, end_of_sequence));
          if (*end_of_sequence) {
            return Status::OK();
          }

          FunctionLibraryRuntime::Options opts;
          // Choose a step ID that is guaranteed not to clash with any
          // Session-generated step ID. DirectSession only generates
          // non-negative step IDs (contiguous, starting from 0), and
          // MasterSession generates 56-bit random step IDs whose MSB
          // is always 0, so a negative random step ID should suffice.
          opts.step_id = -std::abs(static_cast<int64>(random::New64()));
          opts.runner = ctx->runner();
          // TODO(mrry): Avoid blocking a threadpool thread. We will need to
          // stack-rip the iterators and use async kernels.
          Notification n;
          Status ret;
          std::vector<Tensor> result;
          ret = dataset()->captured_func_->Run(opts, *out_tensors, &result);

          if (!ret.ok()) {
            return ret;
          } else if (result.size() != 1 || result[0].dtype() != DT_BOOL ||
                     result[0].NumElements() != 1) {
            return errors::InvalidArgument(
                "Filter predicate `f` must return a scalar bool.");
          }
          matched = result[0].scalar<bool>()();
        } while (!matched);
        *end_of_sequence = false;
        return Status::OK();
      }

     private:
      const std::unique_ptr<IteratorBase> input_impl_;
    };

    const DatasetBase* const input_;
    const std::unique_ptr<CapturedFunction> captured_func_;
  };

 private:
  const NameAttrList* func_;
};

REGISTER_KERNEL_BUILDER(Name("FilterDataset").Device(DEVICE_CPU),
                        FilterDatasetOp);

}  // namespace

}  // namespace tensorflow
