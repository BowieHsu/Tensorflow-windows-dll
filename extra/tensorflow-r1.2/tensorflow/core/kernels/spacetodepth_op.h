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

#ifndef TENSORFLOW_CORE_KERNELS_SPACETODEPTH_OP_H_
#define TENSORFLOW_CORE_KERNELS_SPACETODEPTH_OP_H_
// Functor definition for XentOp, must be compilable by nvcc.

#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"
#include "tensorflow/core/framework/tensor_types.h"

namespace tensorflow {
namespace functor {

// Functor used by SpaceToDepthOp to do the computations.
template <typename Device, typename T>
struct SpaceToDepthOpFunctor {
  // Implements the space to depth conversion.
  //
  // input: 4-D input tensor.
  // block_size: block size for the conversion.
  // output: 4-D output tensor.
  //
  // The dimensions of the tensors are guaranteed to be right when the
  // functor is called.
  void operator()(const Device& d, typename TTypes<T, 4>::ConstTensor input,
                  int block_size, typename TTypes<T, 4>::Tensor output);
};

}  // namespace functor
}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_KERNELS_SPACETODEPTH_OP_H_
