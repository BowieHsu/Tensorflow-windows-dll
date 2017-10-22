// This file is MACHINE GENERATED! Do not edit.


#include "tensorflow/cc/ops/const_op.h"
#include "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/tensorflow/cc/ops/image_ops_internal.h"

namespace tensorflow {
namespace ops {
namespace internal {
// NOTE: This namespace has internal TensorFlow details that
// are not part of TensorFlow's public API.

ResizeBilinearGrad::ResizeBilinearGrad(const ::tensorflow::Scope& scope,
                                       ::tensorflow::Input grads,
                                       ::tensorflow::Input original_image,
                                       const ResizeBilinearGrad::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _grads = ::tensorflow::ops::AsNodeOut(scope, grads);
  if (!scope.ok()) return;
  auto _original_image = ::tensorflow::ops::AsNodeOut(scope, original_image);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ResizeBilinearGrad");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ResizeBilinearGrad")
                     .Input(_grads)
                     .Input(_original_image)
                     .Attr("align_corners", attrs.align_corners_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

ResizeBilinearGrad::ResizeBilinearGrad(const ::tensorflow::Scope& scope,
                                       ::tensorflow::Input grads,
                                       ::tensorflow::Input original_image)
  : ResizeBilinearGrad(scope, grads, original_image, ResizeBilinearGrad::Attrs()) {}

ResizeNearestNeighborGrad::ResizeNearestNeighborGrad(const ::tensorflow::Scope&
                                                     scope, ::tensorflow::Input
                                                     grads, ::tensorflow::Input
                                                     size, const
                                                     ResizeNearestNeighborGrad::Attrs&
                                                     attrs) {
  if (!scope.ok()) return;
  auto _grads = ::tensorflow::ops::AsNodeOut(scope, grads);
  if (!scope.ok()) return;
  auto _size = ::tensorflow::ops::AsNodeOut(scope, size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ResizeNearestNeighborGrad");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ResizeNearestNeighborGrad")
                     .Input(_grads)
                     .Input(_size)
                     .Attr("align_corners", attrs.align_corners_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

ResizeNearestNeighborGrad::ResizeNearestNeighborGrad(const ::tensorflow::Scope&
                                                     scope, ::tensorflow::Input
                                                     grads, ::tensorflow::Input
                                                     size)
  : ResizeNearestNeighborGrad(scope, grads, size, ResizeNearestNeighborGrad::Attrs()) {}

}  // namespace internal
}  // namespace ops
}  // namespace tensorflow
