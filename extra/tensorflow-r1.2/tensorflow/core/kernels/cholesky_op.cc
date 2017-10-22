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

// See docs in ../ops/linalg_ops.cc.
// TODO(konstantinos): Enable complex inputs. This will require additional tests
//                     and OP_REQUIRES.
#if GOOGLE_CUDA
#define EIGEN_USE_GPU
#endif  // GOOGLE_CUDA

#include "third_party/eigen3/Eigen/Cholesky"
#include "third_party/eigen3/Eigen/Core"
#include "tensorflow/core/framework/kernel_def_builder.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/register_types.h"
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/kernels/linalg_ops_common.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"

#if GOOGLE_CUDA
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"
#include "tensorflow/core/kernels/cuda_solvers.h"
#include "tensorflow/core/kernels/matrix_band_part_op.h"
#include "tensorflow/core/platform/stream_executor.h"
#endif

namespace tensorflow {

static const char kErrMsg[] =
    "Cholesky decomposition was not successful. The input might not be valid.";

template <class Scalar>
class CholeskyOp : public LinearAlgebraOp<Scalar> {
 public:
  INHERIT_LINALG_TYPEDEFS(Scalar);

  explicit CholeskyOp(OpKernelConstruction* context) : Base(context) {}

  void ComputeMatrix(OpKernelContext* context, const ConstMatrixMaps& inputs,
                     MatrixMaps* outputs) final {
    const ConstMatrixMap& input = inputs[0];
    if (input.rows() == 0) {
      // If X is an empty matrix (0 rows, 0 col), X * X' == X.
      // Therefore, we return X.
      return;
    }
    // Perform the actual LL^T Cholesky decomposition. This will only use
    // the lower triangular part of data_in by default. The upper triangular
    // part of the matrix will not be read.
    Eigen::LLT<
        Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        llt_decomposition(input);

    // Output the lower triangular in a dense form.
    outputs->at(0) = llt_decomposition.matrixL();

    OP_REQUIRES(context, llt_decomposition.info() == Eigen::Success,
                errors::InvalidArgument(kErrMsg));
  }
};

#if GOOGLE_CUDA
typedef Eigen::GpuDevice GPUDevice;

namespace functor {
#define DECLARE_GPU_SPEC(T)                                                  \
  template <>                                                                \
  void MatrixBandPart<GPUDevice, T>::Compute(                                \
      const GPUDevice& d, Eigen::DenseIndex num_lower,                       \
      Eigen::DenseIndex num_upper, typename TTypes<T, 3>::ConstTensor input, \
      typename TTypes<T, 3>::Tensor output);                                 \
  extern template struct MatrixBandPart<GPUDevice, T>;

TF_CALL_float(DECLARE_GPU_SPEC);
TF_CALL_double(DECLARE_GPU_SPEC);
}  // namespace functor

template <class Scalar>
class CholeskyOpGpu : public AsyncOpKernel {
 public:
  explicit CholeskyOpGpu(OpKernelConstruction* context)
      : AsyncOpKernel(context) {}

  void ComputeAsync(OpKernelContext* context, DoneCallback done) final {
    const Tensor& input = context->input(0);
    const int ndims = input.dims();
    const int64 n = input.dim_size(ndims - 1);
    // Validate inputs.
    OP_REQUIRES_ASYNC(
        context, ndims >= 2,
        errors::InvalidArgument("Input must have rank >= 2, got ", ndims),
        done);
    OP_REQUIRES_ASYNC(
        context, input.dim_size(ndims - 2) == n,
        errors::InvalidArgument("Input matrices must be squares, got",
                                input.dim_size(ndims - 2), " != ", n),
        done);

    // Allocate output.
    Tensor* output;
    OP_REQUIRES_OK_ASYNC(context,
                         context->forward_input_or_allocate_output(
                             {0}, 0, input.shape(), &output),
                         done);

    if (n == 0) {
      // If X is an empty matrix (0 rows, 0 col), X * X' == X.
      // Therefore, we return X.
      done();
      return;
    }

    // Copy the lower triangular part of the input matrices to the output and
    // set the strictly upper triangular part to zero. We use a pre-existing
    // kernel MatrixBandPart to do this for all matrices in the batch at once,
    // before we launch each of the Cholesky factorization kernels in paralle.
    auto input_reshaped = input.template flat_inner_dims<Scalar, 3>();
    auto output_reshaped = output->template flat_inner_dims<Scalar, 3>();
    functor::MatrixBandPart<GPUDevice, Scalar>::Compute(
        context->eigen_device<GPUDevice>(), n, 0, input_reshaped,
        output_reshaped);

    // Launch a Cholesky kernel for each matrix in the batch.
    const int64 batch_size = input_reshaped.dimension(0);
    std::vector<DeviceLapackInfo> dev_info;
    dev_info.emplace_back(context, batch_size, "potrf");
    // TODO(rmlarsen): Parallelize over batches if it turns out to be
    // an important use case.
    CudaSolver solver(context);
    for (int64 i = 0; i < batch_size; ++i) {
      Scalar* output_ptr = output_reshaped.data() + i * n * n;
      int* dev_info_ptr = dev_info.back().mutable_data() + i;
      OP_REQUIRES_OK_ASYNC(
          context,
          solver.Potrf(CUBLAS_FILL_MODE_UPPER, n, output_ptr, n, dev_info_ptr),
          done);
    }

    // Register callback to check info after kernels finish.
    auto info_checker = [context, dev_info, done](
                            const Status& status,
                            const std::vector<HostLapackInfo>& /* unused */) {
      Status full_status = status;
      if (!full_status.ok()) {
        full_status.Update(errors::InvalidArgument(kErrMsg));
      }
      OP_REQUIRES_OK_ASYNC(context, full_status, done);
      done();
    };

    OP_REQUIRES_OK_ASYNC(
        context,
        solver.CopyLapackInfoToHostAsync(dev_info, std::move(info_checker)),
        done);
  }
};

REGISTER_LINALG_OP_GPU("Cholesky", (CholeskyOpGpu<float>), float);
REGISTER_LINALG_OP_GPU("Cholesky", (CholeskyOpGpu<double>), double);

#endif  // GOOGLE_CUDA

REGISTER_LINALG_OP("Cholesky", (CholeskyOp<float>), float);
REGISTER_LINALG_OP("Cholesky", (CholeskyOp<double>), double);
REGISTER_LINALG_OP("BatchCholesky", (CholeskyOp<float>), float);
REGISTER_LINALG_OP("BatchCholesky", (CholeskyOp<double>), double);

}  // namespace tensorflow
