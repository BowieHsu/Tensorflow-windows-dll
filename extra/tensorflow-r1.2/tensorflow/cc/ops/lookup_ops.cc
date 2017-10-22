// This file is MACHINE GENERATED! Do not edit.


#include "tensorflow/cc/ops/const_op.h"
#include "F:/tensorflow-r1.2/tensorflow-r1.2/tensorflow/contrib/cmake/build/tensorflow/cc/ops/lookup_ops.h"

namespace tensorflow {
namespace ops {

HashTableV2::HashTableV2(const ::tensorflow::Scope& scope, DataType key_dtype,
                         DataType value_dtype, const HashTableV2::Attrs& attrs) {
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("HashTableV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "HashTableV2")
                     .Attr("container", attrs.container_)
                     .Attr("shared_name", attrs.shared_name_)
                     .Attr("use_node_name_sharing", attrs.use_node_name_sharing_)
                     .Attr("key_dtype", key_dtype)
                     .Attr("value_dtype", value_dtype)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->table_handle = Output(ret, 0);
}

HashTableV2::HashTableV2(const ::tensorflow::Scope& scope, DataType key_dtype,
                         DataType value_dtype)
  : HashTableV2(scope, key_dtype, value_dtype, HashTableV2::Attrs()) {}

InitializeTableFromTextFileV2::InitializeTableFromTextFileV2(const
                                                             ::tensorflow::Scope&
                                                             scope,
                                                             ::tensorflow::Input
                                                             table_handle,
                                                             ::tensorflow::Input
                                                             filename, int64
                                                             key_index, int64
                                                             value_index, const
                                                             InitializeTableFromTextFileV2::Attrs&
                                                             attrs) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  auto _filename = ::tensorflow::ops::AsNodeOut(scope, filename);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("InitializeTableFromTextFileV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "InitializeTableFromTextFileV2")
                     .Input(_table_handle)
                     .Input(_filename)
                     .Attr("key_index", key_index)
                     .Attr("value_index", value_index)
                     .Attr("vocab_size", attrs.vocab_size_)
                     .Attr("delimiter", attrs.delimiter_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->operation = Operation(ret);
  return;
}

InitializeTableFromTextFileV2::InitializeTableFromTextFileV2(const
                                                             ::tensorflow::Scope&
                                                             scope,
                                                             ::tensorflow::Input
                                                             table_handle,
                                                             ::tensorflow::Input
                                                             filename, int64
                                                             key_index, int64
                                                             value_index)
  : InitializeTableFromTextFileV2(scope, table_handle, filename, key_index, value_index, InitializeTableFromTextFileV2::Attrs()) {}

InitializeTableV2::InitializeTableV2(const ::tensorflow::Scope& scope,
                                     ::tensorflow::Input table_handle,
                                     ::tensorflow::Input keys,
                                     ::tensorflow::Input values) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  auto _keys = ::tensorflow::ops::AsNodeOut(scope, keys);
  if (!scope.ok()) return;
  auto _values = ::tensorflow::ops::AsNodeOut(scope, values);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("InitializeTableV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "InitializeTableV2")
                     .Input(_table_handle)
                     .Input(_keys)
                     .Input(_values)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->operation = Operation(ret);
  return;
}

LookupTableExport::LookupTableExport(const ::tensorflow::Scope& scope,
                                     ::tensorflow::Input table_handle, DataType
                                     Tkeys, DataType Tvalues) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("LookupTableExport");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "LookupTableExport")
                     .Input(_table_handle)
                     .Attr("Tkeys", Tkeys)
                     .Attr("Tvalues", Tvalues)
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

  this->keys = Output(ret, _outputs_range["keys"].first);
  this->values = Output(ret, _outputs_range["values"].first);
}

LookupTableExportV2::LookupTableExportV2(const ::tensorflow::Scope& scope,
                                         ::tensorflow::Input table_handle,
                                         DataType Tkeys, DataType Tvalues) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("LookupTableExportV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "LookupTableExportV2")
                     .Input(_table_handle)
                     .Attr("Tkeys", Tkeys)
                     .Attr("Tvalues", Tvalues)
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

  this->keys = Output(ret, _outputs_range["keys"].first);
  this->values = Output(ret, _outputs_range["values"].first);
}

LookupTableFindV2::LookupTableFindV2(const ::tensorflow::Scope& scope,
                                     ::tensorflow::Input table_handle,
                                     ::tensorflow::Input keys,
                                     ::tensorflow::Input default_value) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  auto _keys = ::tensorflow::ops::AsNodeOut(scope, keys);
  if (!scope.ok()) return;
  auto _default_value = ::tensorflow::ops::AsNodeOut(scope, default_value);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("LookupTableFindV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "LookupTableFindV2")
                     .Input(_table_handle)
                     .Input(_keys)
                     .Input(_default_value)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->values = Output(ret, 0);
}

LookupTableImportV2::LookupTableImportV2(const ::tensorflow::Scope& scope,
                                         ::tensorflow::Input table_handle,
                                         ::tensorflow::Input keys,
                                         ::tensorflow::Input values) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  auto _keys = ::tensorflow::ops::AsNodeOut(scope, keys);
  if (!scope.ok()) return;
  auto _values = ::tensorflow::ops::AsNodeOut(scope, values);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("LookupTableImportV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "LookupTableImportV2")
                     .Input(_table_handle)
                     .Input(_keys)
                     .Input(_values)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->operation = Operation(ret);
  return;
}

LookupTableInsertV2::LookupTableInsertV2(const ::tensorflow::Scope& scope,
                                         ::tensorflow::Input table_handle,
                                         ::tensorflow::Input keys,
                                         ::tensorflow::Input values) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  auto _keys = ::tensorflow::ops::AsNodeOut(scope, keys);
  if (!scope.ok()) return;
  auto _values = ::tensorflow::ops::AsNodeOut(scope, values);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("LookupTableInsertV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "LookupTableInsertV2")
                     .Input(_table_handle)
                     .Input(_keys)
                     .Input(_values)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->operation = Operation(ret);
  return;
}

LookupTableSizeV2::LookupTableSizeV2(const ::tensorflow::Scope& scope,
                                     ::tensorflow::Input table_handle) {
  if (!scope.ok()) return;
  auto _table_handle = ::tensorflow::ops::AsNodeOut(scope, table_handle);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("LookupTableSizeV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "LookupTableSizeV2")
                     .Input(_table_handle)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->size = Output(ret, 0);
}

MutableDenseHashTableV2::MutableDenseHashTableV2(const ::tensorflow::Scope&
                                                 scope, ::tensorflow::Input
                                                 empty_key, DataType
                                                 value_dtype, const
                                                 MutableDenseHashTableV2::Attrs&
                                                 attrs) {
  if (!scope.ok()) return;
  auto _empty_key = ::tensorflow::ops::AsNodeOut(scope, empty_key);
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("MutableDenseHashTableV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "MutableDenseHashTableV2")
                     .Input(_empty_key)
                     .Attr("container", attrs.container_)
                     .Attr("shared_name", attrs.shared_name_)
                     .Attr("use_node_name_sharing", attrs.use_node_name_sharing_)
                     .Attr("value_dtype", value_dtype)
                     .Attr("value_shape", attrs.value_shape_)
                     .Attr("initial_num_buckets", attrs.initial_num_buckets_)
                     .Attr("max_load_factor", attrs.max_load_factor_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->table_handle = Output(ret, 0);
}

MutableDenseHashTableV2::MutableDenseHashTableV2(const ::tensorflow::Scope&
                                                 scope, ::tensorflow::Input
                                                 empty_key, DataType
                                                 value_dtype)
  : MutableDenseHashTableV2(scope, empty_key, value_dtype, MutableDenseHashTableV2::Attrs()) {}

MutableHashTableOfTensorsV2::MutableHashTableOfTensorsV2(const
                                                         ::tensorflow::Scope&
                                                         scope, DataType
                                                         key_dtype, DataType
                                                         value_dtype, const
                                                         MutableHashTableOfTensorsV2::Attrs&
                                                         attrs) {
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("MutableHashTableOfTensorsV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "MutableHashTableOfTensorsV2")
                     .Attr("container", attrs.container_)
                     .Attr("shared_name", attrs.shared_name_)
                     .Attr("use_node_name_sharing", attrs.use_node_name_sharing_)
                     .Attr("key_dtype", key_dtype)
                     .Attr("value_dtype", value_dtype)
                     .Attr("value_shape", attrs.value_shape_)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->table_handle = Output(ret, 0);
}

MutableHashTableOfTensorsV2::MutableHashTableOfTensorsV2(const
                                                         ::tensorflow::Scope&
                                                         scope, DataType
                                                         key_dtype, DataType
                                                         value_dtype)
  : MutableHashTableOfTensorsV2(scope, key_dtype, value_dtype, MutableHashTableOfTensorsV2::Attrs()) {}

MutableHashTableV2::MutableHashTableV2(const ::tensorflow::Scope& scope,
                                       DataType key_dtype, DataType
                                       value_dtype, const
                                       MutableHashTableV2::Attrs& attrs) {
  if (!scope.ok()) return;
  ::tensorflow::Node* ret;
  const auto unique_name = scope.GetUniqueNameForOp("MutableHashTableV2");
  auto builder = ::tensorflow::NodeBuilder(unique_name, "MutableHashTableV2")
                     .Attr("container", attrs.container_)
                     .Attr("shared_name", attrs.shared_name_)
                     .Attr("use_node_name_sharing", attrs.use_node_name_sharing_)
                     .Attr("key_dtype", key_dtype)
                     .Attr("value_dtype", value_dtype)
  ;
  scope.UpdateBuilder(&builder);
  scope.UpdateStatus(builder.Finalize(scope.graph(), &ret));
  if (!scope.ok()) return;
  this->table_handle = Output(ret, 0);
}

MutableHashTableV2::MutableHashTableV2(const ::tensorflow::Scope& scope,
                                       DataType key_dtype, DataType
                                       value_dtype)
  : MutableHashTableV2(scope, key_dtype, value_dtype, MutableHashTableV2::Attrs()) {}

/// @}

}  // namespace ops
}  // namespace tensorflow
