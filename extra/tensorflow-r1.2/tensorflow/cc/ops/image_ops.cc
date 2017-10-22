// This file is MACHINE GENERATED! Do not edit.


#include "tensorflow/cc/ops/const_op.h"
#include "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/tensorflow/cc/ops/image_ops.h"

namespace tensorflow {
namespace ops {

AdjustContrast::AdjustContrast(const ::tensorflow::Scope& scope,
                               ::tensorflow::Input images, ::tensorflow::Input
                               contrast_factor) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _contrast_factor = ::tensorflow::ops::AsNodeOut(scope, contrast_factor);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("AdjustContrast");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "AdjustContrastv2")
                     .Input(_images)
                     .Input(_contrast_factor)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

AdjustHue::AdjustHue(const ::tensorflow::Scope& scope, ::tensorflow::Input
                     images, ::tensorflow::Input delta) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _delta = ::tensorflow::ops::AsNodeOut(scope, delta);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("AdjustHue");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "AdjustHue")
                     .Input(_images)
                     .Input(_delta)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

AdjustSaturation::AdjustSaturation(const ::tensorflow::Scope& scope,
                                   ::tensorflow::Input images,
                                   ::tensorflow::Input scale) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _scale = ::tensorflow::ops::AsNodeOut(scope, scale);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("AdjustSaturation");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "AdjustSaturation")
                     .Input(_images)
                     .Input(_scale)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

CropAndResize::CropAndResize(const ::tensorflow::Scope& scope,
                             ::tensorflow::Input image, ::tensorflow::Input
                             boxes, ::tensorflow::Input box_ind,
                             ::tensorflow::Input crop_size, const
                             CropAndResize::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _image = ::tensorflow::ops::AsNodeOut(scope, image);
  if (!scope.ok()) return;
  auto _boxes = ::tensorflow::ops::AsNodeOut(scope, boxes);
  if (!scope.ok()) return;
  auto _box_ind = ::tensorflow::ops::AsNodeOut(scope, box_ind);
  if (!scope.ok()) return;
  auto _crop_size = ::tensorflow::ops::AsNodeOut(scope, crop_size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("CropAndResize");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "CropAndResize")
                     .Input(_image)
                     .Input(_boxes)
                     .Input(_box_ind)
                     .Input(_crop_size)
                     .Attr("method", attrs.method_)
                     .Attr("extrapolation_value", attrs.extrapolation_value_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->crops = Output(ret, 0);
}

CropAndResize::CropAndResize(const ::tensorflow::Scope& scope,
                             ::tensorflow::Input image, ::tensorflow::Input
                             boxes, ::tensorflow::Input box_ind,
                             ::tensorflow::Input crop_size)
  : CropAndResize(scope, image, boxes, box_ind, crop_size, CropAndResize::Attrs()) {}

CropAndResizeGradBoxes::CropAndResizeGradBoxes(const ::tensorflow::Scope&
                                               scope, ::tensorflow::Input
                                               grads, ::tensorflow::Input
                                               image, ::tensorflow::Input
                                               boxes, ::tensorflow::Input
                                               box_ind, const
                                               CropAndResizeGradBoxes::Attrs&
                                               attrs) {
  if (!scope.ok()) return;
  auto _grads = ::tensorflow::ops::AsNodeOut(scope, grads);
  if (!scope.ok()) return;
  auto _image = ::tensorflow::ops::AsNodeOut(scope, image);
  if (!scope.ok()) return;
  auto _boxes = ::tensorflow::ops::AsNodeOut(scope, boxes);
  if (!scope.ok()) return;
  auto _box_ind = ::tensorflow::ops::AsNodeOut(scope, box_ind);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("CropAndResizeGradBoxes");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "CropAndResizeGradBoxes")
                     .Input(_grads)
                     .Input(_image)
                     .Input(_boxes)
                     .Input(_box_ind)
                     .Attr("method", attrs.method_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

CropAndResizeGradBoxes::CropAndResizeGradBoxes(const ::tensorflow::Scope&
                                               scope, ::tensorflow::Input
                                               grads, ::tensorflow::Input
                                               image, ::tensorflow::Input
                                               boxes, ::tensorflow::Input
                                               box_ind)
  : CropAndResizeGradBoxes(scope, grads, image, boxes, box_ind, CropAndResizeGradBoxes::Attrs()) {}

CropAndResizeGradImage::CropAndResizeGradImage(const ::tensorflow::Scope&
                                               scope, ::tensorflow::Input
                                               grads, ::tensorflow::Input
                                               boxes, ::tensorflow::Input
                                               box_ind, ::tensorflow::Input
                                               image_size, DataType T, const
                                               CropAndResizeGradImage::Attrs&
                                               attrs) {
  if (!scope.ok()) return;
  auto _grads = ::tensorflow::ops::AsNodeOut(scope, grads);
  if (!scope.ok()) return;
  auto _boxes = ::tensorflow::ops::AsNodeOut(scope, boxes);
  if (!scope.ok()) return;
  auto _box_ind = ::tensorflow::ops::AsNodeOut(scope, box_ind);
  if (!scope.ok()) return;
  auto _image_size = ::tensorflow::ops::AsNodeOut(scope, image_size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("CropAndResizeGradImage");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "CropAndResizeGradImage")
                     .Input(_grads)
                     .Input(_boxes)
                     .Input(_box_ind)
                     .Input(_image_size)
                     .Attr("T", T)
                     .Attr("method", attrs.method_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

CropAndResizeGradImage::CropAndResizeGradImage(const ::tensorflow::Scope&
                                               scope, ::tensorflow::Input
                                               grads, ::tensorflow::Input
                                               boxes, ::tensorflow::Input
                                               box_ind, ::tensorflow::Input
                                               image_size, DataType T)
  : CropAndResizeGradImage(scope, grads, boxes, box_ind, image_size, T, CropAndResizeGradImage::Attrs()) {}

DecodeGif::DecodeGif(const ::tensorflow::Scope& scope, ::tensorflow::Input
                     contents) {
  if (!scope.ok()) return;
  auto _contents = ::tensorflow::ops::AsNodeOut(scope, contents);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("DecodeGif");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "DecodeGif")
                     .Input(_contents)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->image = Output(ret, 0);
}

DecodeJpeg::DecodeJpeg(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       contents, const DecodeJpeg::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _contents = ::tensorflow::ops::AsNodeOut(scope, contents);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("DecodeJpeg");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "DecodeJpeg")
                     .Input(_contents)
                     .Attr("channels", attrs.channels_)
                     .Attr("ratio", attrs.ratio_)
                     .Attr("fancy_upscaling", attrs.fancy_upscaling_)
                     .Attr("try_recover_truncated", attrs.try_recover_truncated_)
                     .Attr("acceptable_fraction", attrs.acceptable_fraction_)
                     .Attr("dct_method", attrs.dct_method_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->image = Output(ret, 0);
}

DecodeJpeg::DecodeJpeg(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       contents)
  : DecodeJpeg(scope, contents, DecodeJpeg::Attrs()) {}

DecodePng::DecodePng(const ::tensorflow::Scope& scope, ::tensorflow::Input
                     contents, const DecodePng::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _contents = ::tensorflow::ops::AsNodeOut(scope, contents);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("DecodePng");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "DecodePng")
                     .Input(_contents)
                     .Attr("channels", attrs.channels_)
                     .Attr("dtype", attrs.dtype_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->image = Output(ret, 0);
}

DecodePng::DecodePng(const ::tensorflow::Scope& scope, ::tensorflow::Input
                     contents)
  : DecodePng(scope, contents, DecodePng::Attrs()) {}

DrawBoundingBoxes::DrawBoundingBoxes(const ::tensorflow::Scope& scope,
                                     ::tensorflow::Input images,
                                     ::tensorflow::Input boxes) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _boxes = ::tensorflow::ops::AsNodeOut(scope, boxes);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("DrawBoundingBoxes");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "DrawBoundingBoxes")
                     .Input(_images)
                     .Input(_boxes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

EncodeJpeg::EncodeJpeg(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       image, const EncodeJpeg::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _image = ::tensorflow::ops::AsNodeOut(scope, image);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("EncodeJpeg");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "EncodeJpeg")
                     .Input(_image)
                     .Attr("format", attrs.format_)
                     .Attr("quality", attrs.quality_)
                     .Attr("progressive", attrs.progressive_)
                     .Attr("optimize_size", attrs.optimize_size_)
                     .Attr("chroma_downsampling", attrs.chroma_downsampling_)
                     .Attr("density_unit", attrs.density_unit_)
                     .Attr("x_density", attrs.x_density_)
                     .Attr("y_density", attrs.y_density_)
                     .Attr("xmp_metadata", attrs.xmp_metadata_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->contents = Output(ret, 0);
}

EncodeJpeg::EncodeJpeg(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       image)
  : EncodeJpeg(scope, image, EncodeJpeg::Attrs()) {}

EncodePng::EncodePng(const ::tensorflow::Scope& scope, ::tensorflow::Input
                     image, const EncodePng::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _image = ::tensorflow::ops::AsNodeOut(scope, image);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("EncodePng");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "EncodePng")
                     .Input(_image)
                     .Attr("compression", attrs.compression_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->contents = Output(ret, 0);
}

EncodePng::EncodePng(const ::tensorflow::Scope& scope, ::tensorflow::Input
                     image)
  : EncodePng(scope, image, EncodePng::Attrs()) {}

ExtractGlimpse::ExtractGlimpse(const ::tensorflow::Scope& scope,
                               ::tensorflow::Input input, ::tensorflow::Input
                               size, ::tensorflow::Input offsets, const
                               ExtractGlimpse::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  auto _size = ::tensorflow::ops::AsNodeOut(scope, size);
  if (!scope.ok()) return;
  auto _offsets = ::tensorflow::ops::AsNodeOut(scope, offsets);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ExtractGlimpse");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ExtractGlimpse")
                     .Input(_input)
                     .Input(_size)
                     .Input(_offsets)
                     .Attr("centered", attrs.centered_)
                     .Attr("normalized", attrs.normalized_)
                     .Attr("uniform_noise", attrs.uniform_noise_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->glimpse = Output(ret, 0);
}

ExtractGlimpse::ExtractGlimpse(const ::tensorflow::Scope& scope,
                               ::tensorflow::Input input, ::tensorflow::Input
                               size, ::tensorflow::Input offsets)
  : ExtractGlimpse(scope, input, size, offsets, ExtractGlimpse::Attrs()) {}

HSVToRGB::HSVToRGB(const ::tensorflow::Scope& scope, ::tensorflow::Input
                   images) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("HSVToRGB");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "HSVToRGB")
                     .Input(_images)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

NonMaxSuppression::NonMaxSuppression(const ::tensorflow::Scope& scope,
                                     ::tensorflow::Input boxes,
                                     ::tensorflow::Input scores,
                                     ::tensorflow::Input max_output_size, const
                                     NonMaxSuppression::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _boxes = ::tensorflow::ops::AsNodeOut(scope, boxes);
  if (!scope.ok()) return;
  auto _scores = ::tensorflow::ops::AsNodeOut(scope, scores);
  if (!scope.ok()) return;
  auto _max_output_size = ::tensorflow::ops::AsNodeOut(scope, max_output_size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("NonMaxSuppression");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "NonMaxSuppression")
                     .Input(_boxes)
                     .Input(_scores)
                     .Input(_max_output_size)
                     .Attr("iou_threshold", attrs.iou_threshold_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->selected_indices = Output(ret, 0);
}

NonMaxSuppression::NonMaxSuppression(const ::tensorflow::Scope& scope,
                                     ::tensorflow::Input boxes,
                                     ::tensorflow::Input scores,
                                     ::tensorflow::Input max_output_size)
  : NonMaxSuppression(scope, boxes, scores, max_output_size, NonMaxSuppression::Attrs()) {}

RGBToHSV::RGBToHSV(const ::tensorflow::Scope& scope, ::tensorflow::Input
                   images) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("RGBToHSV");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "RGBToHSV")
                     .Input(_images)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

ResizeArea::ResizeArea(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       images, ::tensorflow::Input size, const
                       ResizeArea::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _size = ::tensorflow::ops::AsNodeOut(scope, size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ResizeArea");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ResizeArea")
                     .Input(_images)
                     .Input(_size)
                     .Attr("align_corners", attrs.align_corners_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->resized_images = Output(ret, 0);
}

ResizeArea::ResizeArea(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       images, ::tensorflow::Input size)
  : ResizeArea(scope, images, size, ResizeArea::Attrs()) {}

ResizeBicubic::ResizeBicubic(const ::tensorflow::Scope& scope,
                             ::tensorflow::Input images, ::tensorflow::Input
                             size, const ResizeBicubic::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _size = ::tensorflow::ops::AsNodeOut(scope, size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ResizeBicubic");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ResizeBicubic")
                     .Input(_images)
                     .Input(_size)
                     .Attr("align_corners", attrs.align_corners_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->resized_images = Output(ret, 0);
}

ResizeBicubic::ResizeBicubic(const ::tensorflow::Scope& scope,
                             ::tensorflow::Input images, ::tensorflow::Input
                             size)
  : ResizeBicubic(scope, images, size, ResizeBicubic::Attrs()) {}

ResizeBilinear::ResizeBilinear(const ::tensorflow::Scope& scope,
                               ::tensorflow::Input images, ::tensorflow::Input
                               size, const ResizeBilinear::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _size = ::tensorflow::ops::AsNodeOut(scope, size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ResizeBilinear");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ResizeBilinear")
                     .Input(_images)
                     .Input(_size)
                     .Attr("align_corners", attrs.align_corners_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->resized_images = Output(ret, 0);
}

ResizeBilinear::ResizeBilinear(const ::tensorflow::Scope& scope,
                               ::tensorflow::Input images, ::tensorflow::Input
                               size)
  : ResizeBilinear(scope, images, size, ResizeBilinear::Attrs()) {}

ResizeNearestNeighbor::ResizeNearestNeighbor(const ::tensorflow::Scope& scope,
                                             ::tensorflow::Input images,
                                             ::tensorflow::Input size, const
                                             ResizeNearestNeighbor::Attrs&
                                             attrs) {
  if (!scope.ok()) return;
  auto _images = ::tensorflow::ops::AsNodeOut(scope, images);
  if (!scope.ok()) return;
  auto _size = ::tensorflow::ops::AsNodeOut(scope, size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ResizeNearestNeighbor");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ResizeNearestNeighbor")
                     .Input(_images)
                     .Input(_size)
                     .Attr("align_corners", attrs.align_corners_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->resized_images = Output(ret, 0);
}

ResizeNearestNeighbor::ResizeNearestNeighbor(const ::tensorflow::Scope& scope,
                                             ::tensorflow::Input images,
                                             ::tensorflow::Input size)
  : ResizeNearestNeighbor(scope, images, size, ResizeNearestNeighbor::Attrs()) {}

SampleDistortedBoundingBox::SampleDistortedBoundingBox(const
                                                       ::tensorflow::Scope&
                                                       scope,
                                                       ::tensorflow::Input
                                                       image_size,
                                                       ::tensorflow::Input
                                                       bounding_boxes, const
                                                       SampleDistortedBoundingBox::Attrs&
                                                       attrs) {
  if (!scope.ok()) return;
  auto _image_size = ::tensorflow::ops::AsNodeOut(scope, image_size);
  if (!scope.ok()) return;
  auto _bounding_boxes = ::tensorflow::ops::AsNodeOut(scope, bounding_boxes);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("SampleDistortedBoundingBox");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "SampleDistortedBoundingBox")
                     .Input(_image_size)
                     .Input(_bounding_boxes)
                     .Attr("seed", attrs.seed_)
                     .Attr("seed2", attrs.seed2_)
                     .Attr("min_object_covered", attrs.min_object_covered_)
                     .Attr("aspect_ratio_range", attrs.aspect_ratio_range_)
                     .Attr("area_range", attrs.area_range_)
                     .Attr("max_attempts", attrs.max_attempts_)
                     .Attr("use_image_if_no_bounding_boxes", attrs.use_image_if_no_bounding_boxes_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  ::tensorflow::NameRangeMap _outputs_range;
  ::tensorflow::Status _status_ = ::tensorflow::NameRangesForNode(ret->def(), ret->op_def(), nullptr, &_outputs_range);
  if (!_status_.ok()) {
    scope.UpdateStatus(_status_);
    return;
  }

  this->begin = Output(ret, _outputs_range["begin"].first);
  this->size = Output(ret, _outputs_range["size"].first);
  this->bboxes = Output(ret, _outputs_range["bboxes"].first);
}

SampleDistortedBoundingBox::SampleDistortedBoundingBox(const
                                                       ::tensorflow::Scope&
                                                       scope,
                                                       ::tensorflow::Input
                                                       image_size,
                                                       ::tensorflow::Input
                                                       bounding_boxes)
  : SampleDistortedBoundingBox(scope, image_size, bounding_boxes, SampleDistortedBoundingBox::Attrs()) {}

/// @}

}  // namespace ops
}  // namespace tensorflow
