/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

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

// See docs in ../ops/parse_ops.cc.

#include <errno.h>
#include <string>

#include "tensorflow/core/framework/kernel_def_builder.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/strings/numbers.h"

namespace tensorflow {

static constexpr char kErrorMessage[] =
    "StringToNumberOp could not correctly convert string: ";

template <typename OutputType>
class StringToNumberOp : public OpKernel {
 public:
  using OpKernel::OpKernel;

  void Compute(OpKernelContext* context) override {
    // This is not a deep copy of the input tensor; they will share the same
    // underlying storage.
    const Tensor* input_tensor;
    OP_REQUIRES_OK(context, context->input("string_tensor", &input_tensor));
    const auto& input_flat = input_tensor->flat<string>();

    Tensor* output_tensor = nullptr;
    OP_REQUIRES_OK(context,
                   context->allocate_output("output", input_tensor->shape(),
                                            &output_tensor));
    auto output_flat = output_tensor->flat<OutputType>();

    for (int i = 0; i < input_flat.size(); ++i) {
      Convert(input_flat(i), &output_flat(i), context);
    }
  }

 private:
  void Convert(const string& s, OutputType* output_data,
               OpKernelContext* context);
};

template <>
void StringToNumberOp<float>::Convert(const string& s, float* output_data,
                                      OpKernelContext* context) {
  OP_REQUIRES(context, strings::safe_strtof(s.c_str(), output_data),
              errors::InvalidArgument(kErrorMessage, s));
}

template <>
void StringToNumberOp<double>::Convert(const string& s, double* output_data,
                                       OpKernelContext* context) {
  OP_REQUIRES(context, strings::safe_strtod(s.c_str(), output_data),
              errors::InvalidArgument(kErrorMessage, s));
}

template <>
void StringToNumberOp<int32>::Convert(const string& s, int32* output_data,
                                      OpKernelContext* context) {
  OP_REQUIRES(context, strings::safe_strto32(s, output_data),
              errors::InvalidArgument(kErrorMessage, s));
}

template <>
void StringToNumberOp<int64>::Convert(const string& s, int64* output_data,
                                      OpKernelContext* context) {
  OP_REQUIRES(context, strings::safe_strto64(s, output_data),
              errors::InvalidArgument(kErrorMessage, s));
}

// Registers the currently supported output types.
#define REGISTER(type)                                           \
  REGISTER_KERNEL_BUILDER(Name("StringToNumber")                 \
                              .Device(DEVICE_CPU)                \
                              .TypeConstraint<type>("out_type"), \
                          StringToNumberOp<type>)
REGISTER(float);
REGISTER(double);
REGISTER(int32);
REGISTER(int64);
#undef REGISTER

}  // namespace tensorflow
