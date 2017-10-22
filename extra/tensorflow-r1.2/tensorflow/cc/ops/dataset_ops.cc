// This file is MACHINE GENERATED! Do not edit.


#include "tensorflow/cc/ops/const_op.h"
#include "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/tensorflow/cc/ops/dataset_ops.h"

namespace tensorflow {
namespace ops {

BatchDataset::BatchDataset(const ::tensorflow::Scope& scope,
                           ::tensorflow::Input input_dataset,
                           ::tensorflow::Input batch_size, const DataTypeSlice&
                           output_types, const
                           gtl::ArraySlice<PartialTensorShape>& output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _batch_size = ::tensorflow::ops::AsNodeOut(scope, batch_size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("BatchDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "BatchDataset")
                     .Input(_input_dataset)
                     .Input(_batch_size)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

DenseToSparseBatchDataset::DenseToSparseBatchDataset(const ::tensorflow::Scope&
                                                     scope, ::tensorflow::Input
                                                     input_dataset,
                                                     ::tensorflow::Input
                                                     batch_size,
                                                     ::tensorflow::Input
                                                     row_shape, const
                                                     DataTypeSlice&
                                                     output_types, const
                                                     gtl::ArraySlice<PartialTensorShape>&
                                                     output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _batch_size = ::tensorflow::ops::AsNodeOut(scope, batch_size);
  if (!scope.ok()) return;
  auto _row_shape = ::tensorflow::ops::AsNodeOut(scope, row_shape);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("DenseToSparseBatchDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "DenseToSparseBatchDataset")
                     .Input(_input_dataset)
                     .Input(_batch_size)
                     .Input(_row_shape)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

FilterDataset::FilterDataset(const ::tensorflow::Scope& scope,
                             ::tensorflow::Input input_dataset,
                             ::tensorflow::InputList other_arguments, const
                             NameAttrList& predicate, const DataTypeSlice&
                             output_types, const
                             gtl::ArraySlice<PartialTensorShape>&
                             output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _other_arguments = ::tensorflow::ops::AsNodeOutList(scope, other_arguments);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("FilterDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "FilterDataset")
                     .Input(_input_dataset)
                     .Input(_other_arguments)
                     .Attr("predicate", predicate)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

FixedLengthRecordDataset::FixedLengthRecordDataset(const ::tensorflow::Scope&
                                                   scope, ::tensorflow::Input
                                                   filenames,
                                                   ::tensorflow::Input
                                                   header_bytes,
                                                   ::tensorflow::Input
                                                   record_bytes,
                                                   ::tensorflow::Input
                                                   footer_bytes) {
  if (!scope.ok()) return;
  auto _filenames = ::tensorflow::ops::AsNodeOut(scope, filenames);
  if (!scope.ok()) return;
  auto _header_bytes = ::tensorflow::ops::AsNodeOut(scope, header_bytes);
  if (!scope.ok()) return;
  auto _record_bytes = ::tensorflow::ops::AsNodeOut(scope, record_bytes);
  if (!scope.ok()) return;
  auto _footer_bytes = ::tensorflow::ops::AsNodeOut(scope, footer_bytes);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("FixedLengthRecordDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "FixedLengthRecordDataset")
                     .Input(_filenames)
                     .Input(_header_bytes)
                     .Input(_record_bytes)
                     .Input(_footer_bytes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

FlatMapDataset::FlatMapDataset(const ::tensorflow::Scope& scope,
                               ::tensorflow::Input input_dataset,
                               ::tensorflow::InputList other_arguments, const
                               NameAttrList& f, const DataTypeSlice&
                               output_types, const
                               gtl::ArraySlice<PartialTensorShape>&
                               output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _other_arguments = ::tensorflow::ops::AsNodeOutList(scope, other_arguments);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("FlatMapDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "FlatMapDataset")
                     .Input(_input_dataset)
                     .Input(_other_arguments)
                     .Attr("f", f)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

GroupByWindowDataset::GroupByWindowDataset(const ::tensorflow::Scope& scope,
                                           ::tensorflow::Input input_dataset,
                                           ::tensorflow::InputList
                                           key_func_other_arguments,
                                           ::tensorflow::InputList
                                           reduce_func_other_arguments,
                                           ::tensorflow::Input window_size,
                                           const NameAttrList& key_func, const
                                           NameAttrList& reduce_func, const
                                           DataTypeSlice& output_types, const
                                           gtl::ArraySlice<PartialTensorShape>&
                                           output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _key_func_other_arguments = ::tensorflow::ops::AsNodeOutList(scope, key_func_other_arguments);
  if (!scope.ok()) return;
  auto _reduce_func_other_arguments = ::tensorflow::ops::AsNodeOutList(scope, reduce_func_other_arguments);
  if (!scope.ok()) return;
  auto _window_size = ::tensorflow::ops::AsNodeOut(scope, window_size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("GroupByWindowDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "GroupByWindowDataset")
                     .Input(_input_dataset)
                     .Input(_key_func_other_arguments)
                     .Input(_reduce_func_other_arguments)
                     .Input(_window_size)
                     .Attr("key_func", key_func)
                     .Attr("reduce_func", reduce_func)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

Iterator::Iterator(const ::tensorflow::Scope& scope, StringPiece shared_name,
                   StringPiece container, const DataTypeSlice& output_types,
                   const gtl::ArraySlice<PartialTensorShape>& output_shapes) {
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("Iterator");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "Iterator")
                     .Attr("shared_name", shared_name)
                     .Attr("container", container)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

IteratorDispose::IteratorDispose(const ::tensorflow::Scope& scope,
                                 ::tensorflow::Input iterator) {
  if (!scope.ok()) return;
  auto _iterator = ::tensorflow::ops::AsNodeOut(scope, iterator);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("IteratorDispose");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "IteratorDispose")
                     .Input(_iterator)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->operation = Operation(ret);
  return;
}

IteratorGetNext::IteratorGetNext(const ::tensorflow::Scope& scope,
                                 ::tensorflow::Input iterator, const
                                 DataTypeSlice& output_types, const
                                 gtl::ArraySlice<PartialTensorShape>&
                                 output_shapes) {
  if (!scope.ok()) return;
  auto _iterator = ::tensorflow::ops::AsNodeOut(scope, iterator);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("IteratorGetNext");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "IteratorGetNext")
                     .Input(_iterator)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  for (int32 i = 0; i < ret->num_outputs(); ++i)
    this->components.push_back(Output(ret, i));
}

MakeIterator::MakeIterator(const ::tensorflow::Scope& scope,
                           ::tensorflow::Input dataset, ::tensorflow::Input
                           iterator) {
  if (!scope.ok()) return;
  auto _dataset = ::tensorflow::ops::AsNodeOut(scope, dataset);
  if (!scope.ok()) return;
  auto _iterator = ::tensorflow::ops::AsNodeOut(scope, iterator);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("MakeIterator");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "MakeIterator")
                     .Input(_dataset)
                     .Input(_iterator)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->operation = Operation(ret);
  return;
}

MapDataset::MapDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                       input_dataset, ::tensorflow::InputList other_arguments,
                       const NameAttrList& f, const DataTypeSlice&
                       output_types, const gtl::ArraySlice<PartialTensorShape>&
                       output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _other_arguments = ::tensorflow::ops::AsNodeOutList(scope, other_arguments);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("MapDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "MapDataset")
                     .Input(_input_dataset)
                     .Input(_other_arguments)
                     .Attr("f", f)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

OneShotIterator::OneShotIterator(const ::tensorflow::Scope& scope, const
                                 NameAttrList& dataset_factory, const
                                 DataTypeSlice& output_types, const
                                 gtl::ArraySlice<PartialTensorShape>&
                                 output_shapes, const OneShotIterator::Attrs&
                                 attrs) {
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("OneShotIterator");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "OneShotIterator")
                     .Attr("dataset_factory", dataset_factory)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
                     .Attr("container", attrs.container_)
                     .Attr("shared_name", attrs.shared_name_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

OneShotIterator::OneShotIterator(const ::tensorflow::Scope& scope, const
                                 NameAttrList& dataset_factory, const
                                 DataTypeSlice& output_types, const
                                 gtl::ArraySlice<PartialTensorShape>&
                                 output_shapes)
  : OneShotIterator(scope, dataset_factory, output_types, output_shapes, OneShotIterator::Attrs()) {}

PaddedBatchDataset::PaddedBatchDataset(const ::tensorflow::Scope& scope,
                                       ::tensorflow::Input input_dataset,
                                       ::tensorflow::Input batch_size,
                                       ::tensorflow::InputList padded_shapes,
                                       ::tensorflow::InputList padding_values,
                                       const
                                       gtl::ArraySlice<PartialTensorShape>&
                                       output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _batch_size = ::tensorflow::ops::AsNodeOut(scope, batch_size);
  if (!scope.ok()) return;
  auto _padded_shapes = ::tensorflow::ops::AsNodeOutList(scope, padded_shapes);
  if (!scope.ok()) return;
  auto _padding_values = ::tensorflow::ops::AsNodeOutList(scope, padding_values);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("PaddedBatchDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "PaddedBatchDataset")
                     .Input(_input_dataset)
                     .Input(_batch_size)
                     .Input(_padded_shapes)
                     .Input(_padding_values)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

ParallelMapDataset::ParallelMapDataset(const ::tensorflow::Scope& scope,
                                       ::tensorflow::Input input_dataset,
                                       ::tensorflow::InputList other_arguments,
                                       ::tensorflow::Input num_threads,
                                       ::tensorflow::Input output_buffer_size,
                                       const NameAttrList& f, const
                                       DataTypeSlice& output_types, const
                                       gtl::ArraySlice<PartialTensorShape>&
                                       output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _other_arguments = ::tensorflow::ops::AsNodeOutList(scope, other_arguments);
  if (!scope.ok()) return;
  auto _num_threads = ::tensorflow::ops::AsNodeOut(scope, num_threads);
  if (!scope.ok()) return;
  auto _output_buffer_size = ::tensorflow::ops::AsNodeOut(scope, output_buffer_size);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ParallelMapDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ParallelMapDataset")
                     .Input(_input_dataset)
                     .Input(_other_arguments)
                     .Input(_num_threads)
                     .Input(_output_buffer_size)
                     .Attr("f", f)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

RangeDataset::RangeDataset(const ::tensorflow::Scope& scope,
                           ::tensorflow::Input start, ::tensorflow::Input stop,
                           ::tensorflow::Input step, const DataTypeSlice&
                           output_types, const
                           gtl::ArraySlice<PartialTensorShape>& output_shapes) {
  if (!scope.ok()) return;
  auto _start = ::tensorflow::ops::AsNodeOut(scope, start);
  if (!scope.ok()) return;
  auto _stop = ::tensorflow::ops::AsNodeOut(scope, stop);
  if (!scope.ok()) return;
  auto _step = ::tensorflow::ops::AsNodeOut(scope, step);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("RangeDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "RangeDataset")
                     .Input(_start)
                     .Input(_stop)
                     .Input(_step)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

RepeatDataset::RepeatDataset(const ::tensorflow::Scope& scope,
                             ::tensorflow::Input input_dataset,
                             ::tensorflow::Input count, const DataTypeSlice&
                             output_types, const
                             gtl::ArraySlice<PartialTensorShape>&
                             output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _count = ::tensorflow::ops::AsNodeOut(scope, count);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("RepeatDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "RepeatDataset")
                     .Input(_input_dataset)
                     .Input(_count)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

ShuffleDataset::ShuffleDataset(const ::tensorflow::Scope& scope,
                               ::tensorflow::Input input_dataset,
                               ::tensorflow::Input buffer_size,
                               ::tensorflow::Input seed, ::tensorflow::Input
                               seed2, const DataTypeSlice& output_types, const
                               gtl::ArraySlice<PartialTensorShape>&
                               output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _buffer_size = ::tensorflow::ops::AsNodeOut(scope, buffer_size);
  if (!scope.ok()) return;
  auto _seed = ::tensorflow::ops::AsNodeOut(scope, seed);
  if (!scope.ok()) return;
  auto _seed2 = ::tensorflow::ops::AsNodeOut(scope, seed2);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ShuffleDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ShuffleDataset")
                     .Input(_input_dataset)
                     .Input(_buffer_size)
                     .Input(_seed)
                     .Input(_seed2)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

SkipDataset::SkipDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                         input_dataset, ::tensorflow::Input count, const
                         DataTypeSlice& output_types, const
                         gtl::ArraySlice<PartialTensorShape>& output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _count = ::tensorflow::ops::AsNodeOut(scope, count);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("SkipDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "SkipDataset")
                     .Input(_input_dataset)
                     .Input(_count)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

SparseTensorSliceDataset::SparseTensorSliceDataset(const ::tensorflow::Scope&
                                                   scope, ::tensorflow::Input
                                                   indices, ::tensorflow::Input
                                                   values, ::tensorflow::Input
                                                   dense_shape) {
  if (!scope.ok()) return;
  auto _indices = ::tensorflow::ops::AsNodeOut(scope, indices);
  if (!scope.ok()) return;
  auto _values = ::tensorflow::ops::AsNodeOut(scope, values);
  if (!scope.ok()) return;
  auto _dense_shape = ::tensorflow::ops::AsNodeOut(scope, dense_shape);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("SparseTensorSliceDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "SparseTensorSliceDataset")
                     .Input(_indices)
                     .Input(_values)
                     .Input(_dense_shape)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

TFRecordDataset::TFRecordDataset(const ::tensorflow::Scope& scope,
                                 ::tensorflow::Input filenames,
                                 ::tensorflow::Input compression_type) {
  if (!scope.ok()) return;
  auto _filenames = ::tensorflow::ops::AsNodeOut(scope, filenames);
  if (!scope.ok()) return;
  auto _compression_type = ::tensorflow::ops::AsNodeOut(scope, compression_type);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("TFRecordDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "TFRecordDataset")
                     .Input(_filenames)
                     .Input(_compression_type)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

TakeDataset::TakeDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                         input_dataset, ::tensorflow::Input count, const
                         DataTypeSlice& output_types, const
                         gtl::ArraySlice<PartialTensorShape>& output_shapes) {
  if (!scope.ok()) return;
  auto _input_dataset = ::tensorflow::ops::AsNodeOut(scope, input_dataset);
  if (!scope.ok()) return;
  auto _count = ::tensorflow::ops::AsNodeOut(scope, count);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("TakeDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "TakeDataset")
                     .Input(_input_dataset)
                     .Input(_count)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

TensorDataset::TensorDataset(const ::tensorflow::Scope& scope,
                             ::tensorflow::InputList components, const
                             gtl::ArraySlice<PartialTensorShape>&
                             output_shapes) {
  if (!scope.ok()) return;
  auto _components = ::tensorflow::ops::AsNodeOutList(scope, components);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("TensorDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "TensorDataset")
                     .Input(_components)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

TensorSliceDataset::TensorSliceDataset(const ::tensorflow::Scope& scope,
                                       ::tensorflow::InputList components,
                                       const
                                       gtl::ArraySlice<PartialTensorShape>&
                                       output_shapes) {
  if (!scope.ok()) return;
  auto _components = ::tensorflow::ops::AsNodeOutList(scope, components);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("TensorSliceDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "TensorSliceDataset")
                     .Input(_components)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

TextLineDataset::TextLineDataset(const ::tensorflow::Scope& scope,
                                 ::tensorflow::Input filenames) {
  if (!scope.ok()) return;
  auto _filenames = ::tensorflow::ops::AsNodeOut(scope, filenames);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("TextLineDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "TextLineDataset")
                     .Input(_filenames)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

ZipDataset::ZipDataset(const ::tensorflow::Scope& scope,
                       ::tensorflow::InputList input_datasets, const
                       DataTypeSlice& output_types, const
                       gtl::ArraySlice<PartialTensorShape>& output_shapes) {
  if (!scope.ok()) return;
  auto _input_datasets = ::tensorflow::ops::AsNodeOutList(scope, input_datasets);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("ZipDataset");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "ZipDataset")
                     .Input(_input_datasets)
                     .Attr("output_types", output_types)
                     .Attr("output_shapes", output_shapes)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->handle = Output(ret, 0);
}

/// @}

}  // namespace ops
}  // namespace tensorflow
