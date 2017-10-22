// This file is MACHINE GENERATED! Do not edit.


#include "tensorflow/cc/ops/const_op.h"
#include "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/tensorflow/cc/ops/string_ops.h"

namespace tensorflow {
namespace ops {

AsString::AsString(const ::tensorflow::Scope& scope, ::tensorflow::Input input,
                   const AsString::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("AsString");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "AsString")
                     .Input(_input)
                     .Attr("precision", attrs.precision_)
                     .Attr("scientific", attrs.scientific_)
                     .Attr("shortest", attrs.shortest_)
                     .Attr("width", attrs.width_)
                     .Attr("fill", attrs.fill_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

AsString::AsString(const ::tensorflow::Scope& scope, ::tensorflow::Input input)
  : AsString(scope, input, AsString::Attrs()) {}

DecodeBase64::DecodeBase64(const ::tensorflow::Scope& scope,
                           ::tensorflow::Input input) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("DecodeBase64");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "DecodeBase64")
                     .Input(_input)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

EncodeBase64::EncodeBase64(const ::tensorflow::Scope& scope,
                           ::tensorflow::Input input, const
                           EncodeBase64::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("EncodeBase64");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "EncodeBase64")
                     .Input(_input)
                     .Attr("pad", attrs.pad_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

EncodeBase64::EncodeBase64(const ::tensorflow::Scope& scope,
                           ::tensorflow::Input input)
  : EncodeBase64(scope, input, EncodeBase64::Attrs()) {}

ReduceJoin::ReduceJoin(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       inputs, ::tensorflow::Input reduction_indices, const
                       ReduceJoin::Attrs& attrs) {
  if (!scope.ok()) return;
  auto _inputs = ::tensorflow::ops::AsNodeOut(scope, inputs);
  if (!scope.ok()) return;
  auto _reduction_indices = ::tensorflow::ops::AsNodeOut(scope, reduction_indices);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ReduceJoin");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ReduceJoin")
                     .Input(_inputs)
                     .Input(_reduction_indices)
                     .Attr("keep_dims", attrs.keep_dims_)
                     .Attr("separator", attrs.separator_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

ReduceJoin::ReduceJoin(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       inputs, ::tensorflow::Input reduction_indices)
  : ReduceJoin(scope, inputs, reduction_indices, ReduceJoin::Attrs()) {}

StringJoin::StringJoin(const ::tensorflow::Scope& scope,
                       ::tensorflow::InputList inputs, const StringJoin::Attrs&
                       attrs) {
  if (!scope.ok()) return;
  auto _inputs = ::tensorflow::ops::AsNodeOutList(scope, inputs);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("StringJoin");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "StringJoin")
                     .Input(_inputs)
                     .Attr("separator", attrs.separator_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

StringJoin::StringJoin(const ::tensorflow::Scope& scope,
                       ::tensorflow::InputList inputs)
  : StringJoin(scope, inputs, StringJoin::Attrs()) {}

StringSplit::StringSplit(const ::tensorflow::Scope& scope, ::tensorflow::Input
                         input, ::tensorflow::Input delimiter) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  auto _delimiter = ::tensorflow::ops::AsNodeOut(scope, delimiter);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("StringSplit");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "StringSplit")
                     .Input(_input)
                     .Input(_delimiter)
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

  this->indices = Output(ret, _outputs_range["indices"].first);
  this->values = Output(ret, _outputs_range["values"].first);
  this->shape = Output(ret, _outputs_range["shape"].first);
}

StringToHashBucket::StringToHashBucket(const ::tensorflow::Scope& scope,
                                       ::tensorflow::Input string_tensor, int64
                                       num_buckets) {
  if (!scope.ok()) return;
  auto _string_tensor = ::tensorflow::ops::AsNodeOut(scope, string_tensor);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("StringToHashBucket");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "StringToHashBucket")
                     .Input(_string_tensor)
                     .Attr("num_buckets", num_buckets)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

StringToHashBucketFast::StringToHashBucketFast(const ::tensorflow::Scope&
                                               scope, ::tensorflow::Input
                                               input, int64 num_buckets) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("StringToHashBucketFast");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "StringToHashBucketFast")
                     .Input(_input)
                     .Attr("num_buckets", num_buckets)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

StringToHashBucketStrong::StringToHashBucketStrong(const ::tensorflow::Scope&
                                                   scope, ::tensorflow::Input
                                                   input, int64 num_buckets,
                                                   const gtl::ArraySlice<int>&
                                                   key) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("StringToHashBucketStrong");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "StringToHashBucketStrong")
                     .Input(_input)
                     .Attr("num_buckets", num_buckets)
                     .Attr("key", key)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

Substr::Substr(const ::tensorflow::Scope& scope, ::tensorflow::Input input,
               ::tensorflow::Input pos, ::tensorflow::Input len) {
  if (!scope.ok()) return;
  auto _input = ::tensorflow::ops::AsNodeOut(scope, input);
  if (!scope.ok()) return;
  auto _pos = ::tensorflow::ops::AsNodeOut(scope, pos);
  if (!scope.ok()) return;
  auto _len = ::tensorflow::ops::AsNodeOut(scope, len);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("Substr");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "Substr")
                     .Input(_input)
                     .Input(_pos)
                     .Input(_len)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->output = Output(ret, 0);
}

/// @}

}  // namespace ops
}  // namespace tensorflow
