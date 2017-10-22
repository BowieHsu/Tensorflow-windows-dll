// This file is MACHINE GENERATED! Do not edit.

#ifndef F__TENSORFLOW_R____TENSORFLOW_R____TENSORFLOW_CONTRIB_CMAKE_BUILD_TENSORFLOW_CC_OPS_IMAGE_OPS_INTERNAL_H_
#define F__TENSORFLOW_R____TENSORFLOW_R____TENSORFLOW_CONTRIB_CMAKE_BUILD_TENSORFLOW_CC_OPS_IMAGE_OPS_INTERNAL_H_

// This file is MACHINE GENERATED! Do not edit.

#include "tensorflow/cc/framework/ops.h"
#include "tensorflow/cc/framework/scope.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/framework/types.h"
#include "tensorflow/core/lib/gtl/array_slice.h"

namespace tensorflow {
namespace ops {
namespace internal {
// NOTE: This namespace has internal TensorFlow details that
// are not part of TensorFlow's public API.

/// @defgroup image_ops_internal Image Ops Internal
/// @{

/// Computes the gradient of bilinear interpolation.
///
/// Arguments:
/// * scope: A Scope object
/// * grads: 4-D with shape `[batch, height, width, channels]`.
/// * original_image: 4-D with shape `[batch, orig_height, orig_width, channels]`,
/// The image tensor that was resized.
///
/// Optional attributes (see `Attrs`):
/// * align_corners: If true, rescale grads by (orig_height - 1) / (height - 1), which
/// exactly aligns the 4 corners of grads and original_image. If false, rescale by
/// orig_height / height. Treat similarly the width dimension.
///
/// Returns:
/// * `Output`: 4-D with shape `[batch, orig_height, orig_width, channels]`.
/// Gradients with respect to the input image. Input image must have been
/// float or double.
class ResizeBilinearGrad {
 public:
  /// Optional attribute setters for ResizeBilinearGrad
  struct Attrs {
    /// If true, rescale grads by (orig_height - 1) / (height - 1), which
    /// exactly aligns the 4 corners of grads and original_image. If false, rescale by
    /// orig_height / height. Treat similarly the width dimension.
    ///
    /// Defaults to false
    Attrs AlignCorners(bool x) {
      Attrs ret = *this;
      ret.align_corners_ = x;
      return ret;
    }

    bool align_corners_ = false;
  };
  ResizeBilinearGrad(const ::tensorflow::Scope& scope, ::tensorflow::Input grads,
                   ::tensorflow::Input original_image);
  ResizeBilinearGrad(const ::tensorflow::Scope& scope, ::tensorflow::Input grads,
                   ::tensorflow::Input original_image, const
                   ResizeBilinearGrad::Attrs& attrs);
  operator ::tensorflow::Output() const { return output; }
  operator ::tensorflow::Input() const { return output; }
  ::tensorflow::Node* node() const { return output.node(); }

  static Attrs AlignCorners(bool x) {
    return Attrs().AlignCorners(x);
  }

  ::tensorflow::Output output;
};

/// Computes the gradient of nearest neighbor interpolation.
///
/// Arguments:
/// * scope: A Scope object
/// * grads: 4-D with shape `[batch, height, width, channels]`.
/// * size: = A 1-D int32 Tensor of 2 elements: `orig_height, orig_width`. The
/// original input size.
///
/// Optional attributes (see `Attrs`):
/// * align_corners: If true, rescale grads by (orig_height - 1) / (height - 1), which
/// exactly aligns the 4 corners of grads and original_image. If false, rescale by
/// orig_height / height. Treat similarly the width dimension.
///
/// Returns:
/// * `Output`: 4-D with shape `[batch, orig_height, orig_width, channels]`. Gradients
/// with respect to the input image.
class ResizeNearestNeighborGrad {
 public:
  /// Optional attribute setters for ResizeNearestNeighborGrad
  struct Attrs {
    /// If true, rescale grads by (orig_height - 1) / (height - 1), which
    /// exactly aligns the 4 corners of grads and original_image. If false, rescale by
    /// orig_height / height. Treat similarly the width dimension.
    ///
    /// Defaults to false
    Attrs AlignCorners(bool x) {
      Attrs ret = *this;
      ret.align_corners_ = x;
      return ret;
    }

    bool align_corners_ = false;
  };
  ResizeNearestNeighborGrad(const ::tensorflow::Scope& scope, ::tensorflow::Input
                          grads, ::tensorflow::Input size);
  ResizeNearestNeighborGrad(const ::tensorflow::Scope& scope, ::tensorflow::Input
                          grads, ::tensorflow::Input size, const
                          ResizeNearestNeighborGrad::Attrs& attrs);
  operator ::tensorflow::Output() const { return output; }
  operator ::tensorflow::Input() const { return output; }
  ::tensorflow::Node* node() const { return output.node(); }

  static Attrs AlignCorners(bool x) {
    return Attrs().AlignCorners(x);
  }

  ::tensorflow::Output output;
};

}  // namespace internal
}  // namespace ops
}  // namespace tensorflow

#endif  // F__TENSORFLOW_R____TENSORFLOW_R____TENSORFLOW_CONTRIB_CMAKE_BUILD_TENSORFLOW_CC_OPS_IMAGE_OPS_INTERNAL_H_
