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

#ifndef THIRD_PARTY_TENSORFLOW_CORE_KERNELS_GATHER_FUNCTOR_GPU_CU_H_
#define THIRD_PARTY_TENSORFLOW_CORE_KERNELS_GATHER_FUNCTOR_GPU_CU_H_

#if GOOGLE_CUDA

#define EIGEN_USE_GPU

#include "tensorflow/core/framework/register_types.h"
#include "tensorflow/core/kernels/gather_functor.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/util/cuda_kernel_helper.h"

namespace tensorflow {

typedef Eigen::GpuDevice GPUDevice;

template <typename T, typename Index>
__global__ void GatherOpKernel(const T* params, const Index* indices, T* out,
                               int64 first_dim_size, int64 indices_size,
                               int64 out_size) {
  const int32 slice_size = out_size / indices_size;
  CUDA_1D_KERNEL_LOOP(i, out_size) {
    Index indices_i = i / slice_size;
    Index indices_slice_i = i - indices_i * slice_size;
    Index params_first_index = ldg(indices + indices_i);
    if (!(params_first_index >= 0 && params_first_index < first_dim_size)) {
      // Set indices out of range to zero
      // TODO(fpmc): Log an error for transfer back to host.
      out[i] = T(0);
    } else {
      Index params_i = params_first_index * slice_size + indices_slice_i;
      out[i] = ldg(params + params_i);
    }
  }
}

namespace functor {
template <typename T, typename Index>
struct GatherFunctor<GPUDevice, T, Index> {
  int64 operator()(const GPUDevice& d, typename TTypes<T>::ConstMatrix params,
                   typename TTypes<Index>::ConstFlat indices,
                   typename TTypes<T>::Matrix out) {
    const int64 out_size = out.size();
    if (out_size == 0) {
      // We need a check here since the CPU version does useful error checking
      // work if there are nonempty indices but empty slices, so the kernel is
      // executed in that case.  In the GPU case we don't know how to do error
      // checking, so we skip the loop entirely.
      return -1;
    }
    const int64 first_dim_size = params.dimension(0);
    const int64 indices_size = indices.size();
    CudaLaunchConfig config = GetCudaLaunchConfig(out_size, d);
    // clang-format off
    GatherOpKernel<T, Index>
        <<<config.block_count, config.thread_per_block, 0, d.stream()>>>(
            params.data(), indices.data(), out.data(), first_dim_size,
            indices_size, out_size);
    // clang-format on
    // TODO(fpmc): enable indices validation on GPU.
    // Right now checking for indicies out of bound in the kernel would
    // require copying code between GPU/CPU, and thus slow.
    return -1;
  }
};

}  // namespace functor
}  // namespace tensorflow

#endif  // GOOGLE_CUDA

#endif  // THIRD_PARTY_TENSORFLOW_CORE_KERNELS_GATHER_FUNCTOR_GPU_CU_H_
