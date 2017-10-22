// This file is MACHINE GENERATED! Do not edit.

#ifndef F__TENSORFLOW_R____TENSORFLOW_R____TENSORFLOW_CONTRIB_CMAKE_BUILD_TENSORFLOW_CC_OPS_DATASET_OPS_H_
#define F__TENSORFLOW_R____TENSORFLOW_R____TENSORFLOW_CONTRIB_CMAKE_BUILD_TENSORFLOW_CC_OPS_DATASET_OPS_H_

// This file is MACHINE GENERATED! Do not edit.

#include "tensorflow/cc/framework/ops.h"
#include "tensorflow/cc/framework/scope.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/framework/types.h"
#include "tensorflow/core/lib/gtl/array_slice.h"

namespace tensorflow {
namespace ops {

/// @defgroup dataset_ops Dataset Ops
/// @{

/// Creates a dataset that batches `batch_size` elements from `input_dataset`.
///
/// Arguments:
/// * scope: A Scope object
/// * batch_size: A scalar representing the number of elements to accumulate in a
/// batch.
///
/// Returns:
/// * `Output`: The handle tensor.
class BatchDataset {
 public:
  BatchDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
             input_dataset, ::tensorflow::Input batch_size, const
             DataTypeSlice& output_types, const
             gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that yields a SparseTensor for each element of the input.
///
/// Arguments:
/// * scope: A Scope object
/// * input_dataset: A handle to an input dataset. Must have a single component.
/// * batch_size: A scalar representing the number of elements to accumulate in a
/// batch.
/// * row_shape: A vector representing the dense shape of each row in the produced
/// SparseTensor.
///
/// Returns:
/// * `Output`: The handle tensor.
class DenseToSparseBatchDataset {
 public:
  DenseToSparseBatchDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                          input_dataset, ::tensorflow::Input batch_size,
                          ::tensorflow::Input row_shape, const DataTypeSlice&
                          output_types, const
                          gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset containing elements of `input_dataset` matching `predicate`.
///
/// The `predicate` function must return a scalar boolean and accept the
/// following arguments:
///
/// * One tensor for each component of an element of `input_dataset`.
/// * One tensor for each value in `other_arguments`.
///
/// Arguments:
/// * scope: A Scope object
/// * other_arguments: A list of tensors, typically values that were captured when
/// building a closure for `predicate`.
/// * predicate: A function returning a scalar boolean.
///
/// Returns:
/// * `Output`: The handle tensor.
class FilterDataset {
 public:
  FilterDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
              input_dataset, ::tensorflow::InputList other_arguments, const
              NameAttrList& predicate, const DataTypeSlice& output_types, const
              gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that emits the records from one or more binary files.
///
/// Arguments:
/// * scope: A Scope object
/// * filenames: A scalar or a vector containing the name(s) of the file(s) to be
/// read.
/// * header_bytes: A scalar representing the number of bytes to skip at the
/// beginning of a file.
/// * record_bytes: A scalar representing the number of bytes in each record.
/// * footer_bytes: A scalar representing the number of bytes to skip at the end
/// of a file.
///
/// Returns:
/// * `Output`: The handle tensor.
class FixedLengthRecordDataset {
 public:
  FixedLengthRecordDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                         filenames, ::tensorflow::Input header_bytes,
                         ::tensorflow::Input record_bytes, ::tensorflow::Input
                         footer_bytes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that applies `f` to the outputs of `input_dataset`.
///
/// Unlike MapDataset, the `f` in FlatMapDataset is expected to return a
/// Dataset resource, and FlatMapDataset will flatten successive results
/// into a single Dataset.
///
/// Arguments:
/// * scope: A Scope object
/// * f: A function mapping elements of `input_dataset`, concatenated with
/// `other_arguments`, to a Dataset resource that contains elements matching
/// `output_types` and `output_shapes`.
///
/// Returns:
/// * `Output`: The handle tensor.
class FlatMapDataset {
 public:
  FlatMapDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
               input_dataset, ::tensorflow::InputList other_arguments, const
               NameAttrList& f, const DataTypeSlice& output_types, const
               gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that computes a windowed group-by on `input_dataset`.
///
/// // TODO(mrry): Support non-int64 keys.
///
/// Arguments:
/// * scope: A Scope object
/// * key_func: A function mapping an element of `input_dataset`, concatenated
/// with `key_func_other_arguments` to a scalar value of type DT_INT64.
///
/// Returns:
/// * `Output`: The handle tensor.
class GroupByWindowDataset {
 public:
  GroupByWindowDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                     input_dataset, ::tensorflow::InputList
                     key_func_other_arguments, ::tensorflow::InputList
                     reduce_func_other_arguments, ::tensorflow::Input
                     window_size, const NameAttrList& key_func, const
                     NameAttrList& reduce_func, const DataTypeSlice&
                     output_types, const gtl::ArraySlice<PartialTensorShape>&
                     output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// A container for an iterator resource.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * `Output`: A handle to the iterator that can be passed to a "MakeIterator"
/// or "IteratorGetNext" op.
class Iterator {
 public:
  Iterator(const ::tensorflow::Scope& scope, StringPiece shared_name, StringPiece
         container, const DataTypeSlice& output_types, const
         gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Releases any resources used by the given iterator.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * the created `Operation`
class IteratorDispose {
 public:
  IteratorDispose(const ::tensorflow::Scope& scope, ::tensorflow::Input iterator);
  operator ::tensorflow::Operation() const { return operation; }

  Operation operation;
};

/// Gets the next output from the given iterator.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * `OutputList`: The components tensor.
class IteratorGetNext {
 public:
  IteratorGetNext(const ::tensorflow::Scope& scope, ::tensorflow::Input iterator,
                const DataTypeSlice& output_types, const
                gtl::ArraySlice<PartialTensorShape>& output_shapes);
  ::tensorflow::Output operator[](size_t index) const { return components[index]; }


  ::tensorflow::OutputList components;
};

/// Makes a new iterator from the given `dataset` and stores it in `iterator`.
///
/// This operation may be executed multiple times. Each execution will reset the
/// iterator in `iterator` to the first element of `dataset`.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * the created `Operation`
class MakeIterator {
 public:
  MakeIterator(const ::tensorflow::Scope& scope, ::tensorflow::Input dataset,
             ::tensorflow::Input iterator);
  operator ::tensorflow::Operation() const { return operation; }

  Operation operation;
};

/// Creates a dataset that applies `f` to the outputs of `input_dataset`.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * `Output`: The handle tensor.
class MapDataset {
 public:
  MapDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input input_dataset,
           ::tensorflow::InputList other_arguments, const NameAttrList& f,
           const DataTypeSlice& output_types, const
           gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Makes a "one-shot" iterator that can be iterated only once.
///
/// A one-shot iterator bundles the logic for defining the dataset and
/// the state of the iterator in a single op, which allows simple input
/// pipelines to be defined without an additional initialization
/// ("MakeIterator") step.
///
/// One-shot iterators have the following limitations:
///
/// * They do not support parameterization: all logic for creating the underlying
///   dataset must be bundled in the `dataset_factory` function.
/// * They are not resettable. Once a one-shot iterator reaches the end of its
///   underlying dataset, subsequent "IteratorGetNext" operations on that
///   iterator will always produce an `OutOfRange` error.
///
/// For greater flexibility, use "Iterator" and "MakeIterator" to define
/// an iterator using an arbitrary subgraph, which may capture tensors
/// (including fed values) as parameters, and which may be reset multiple
/// times by rerunning "MakeIterator".
///
/// Arguments:
/// * scope: A Scope object
/// * dataset_factory: A function of type `() -> DT_RESOURCE`, where the returned
/// DT_RESOURCE is a handle to a dataset.
///
/// Returns:
/// * `Output`: A handle to the iterator that can be passed to an "IteratorGetNext"
/// op.
class OneShotIterator {
 public:
  /// Optional attribute setters for OneShotIterator
  struct Attrs {
    /// Defaults to ""
    Attrs Container(StringPiece x) {
      Attrs ret = *this;
      ret.container_ = x;
      return ret;
    }

    /// Defaults to ""
    Attrs SharedName(StringPiece x) {
      Attrs ret = *this;
      ret.shared_name_ = x;
      return ret;
    }

    StringPiece container_ = "";
    StringPiece shared_name_ = "";
  };
  OneShotIterator(const ::tensorflow::Scope& scope, const NameAttrList&
                dataset_factory, const DataTypeSlice& output_types, const
                gtl::ArraySlice<PartialTensorShape>& output_shapes);
  OneShotIterator(const ::tensorflow::Scope& scope, const NameAttrList&
                dataset_factory, const DataTypeSlice& output_types, const
                gtl::ArraySlice<PartialTensorShape>& output_shapes, const
                OneShotIterator::Attrs& attrs);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  static Attrs Container(StringPiece x) {
    return Attrs().Container(x);
  }
  static Attrs SharedName(StringPiece x) {
    return Attrs().SharedName(x);
  }

  ::tensorflow::Output handle;
};

/// Creates a dataset that batches and pads `batch_size` elements from the input.
///
/// Arguments:
/// * scope: A Scope object
/// * batch_size: A scalar representing the number of elements to accumulate in a
/// batch.
/// * padded_shapes: A list of int64 tensors representing the desired padded shapes
/// of the corresponding output components. These shapes may be partially
/// specified, using `-1` to indicate that a particular dimension should be
/// padded to the maximum size of all batch elements.
/// * padding_values: A list of scalars containing the padding value to use for
/// each of the outputs.
///
/// Returns:
/// * `Output`: The handle tensor.
class PaddedBatchDataset {
 public:
  PaddedBatchDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                   input_dataset, ::tensorflow::Input batch_size,
                   ::tensorflow::InputList padded_shapes,
                   ::tensorflow::InputList padding_values, const
                   gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that applies `f` to the outputs of `input_dataset`.
///
/// Unlike a "MapDataset", which applies `f` sequentially, this dataset uses
/// up to `num_threads` threads to process elements from `input_dataset`
/// in parallel.
///
/// Arguments:
/// * scope: A Scope object
/// * num_threads: The number of threads to use to process elements from
/// `input_dataset`.
/// * output_buffer_size: The maximum number of output elements to buffer in an
/// iterator over this dataset.
///
/// Returns:
/// * `Output`: The handle tensor.
class ParallelMapDataset {
 public:
  ParallelMapDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                   input_dataset, ::tensorflow::InputList other_arguments,
                   ::tensorflow::Input num_threads, ::tensorflow::Input
                   output_buffer_size, const NameAttrList& f, const
                   DataTypeSlice& output_types, const
                   gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset with a range of values. Corresponds to python's xrange.
///
/// Arguments:
/// * scope: A Scope object
/// * start: corresponds to start in python's xrange().
/// * stop: corresponds to stop in python's xrange().
/// * step: corresponds to step in python's xrange().
///
/// Returns:
/// * `Output`: The handle tensor.
class RangeDataset {
 public:
  RangeDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input start,
             ::tensorflow::Input stop, ::tensorflow::Input step, const
             DataTypeSlice& output_types, const
             gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that emits the outputs of `input_dataset` `count` times.
///
/// Arguments:
/// * scope: A Scope object
/// * count: A scalar representing the number of times that `input_dataset` should
/// be repeated. A value of `-1` indicates that it should be repeated infinitely.
///
/// Returns:
/// * `Output`: The handle tensor.
class RepeatDataset {
 public:
  RepeatDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
              input_dataset, ::tensorflow::Input count, const DataTypeSlice&
              output_types, const gtl::ArraySlice<PartialTensorShape>&
              output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that shuffles elements from `input_dataset` pseudorandomly.
///
/// Arguments:
/// * scope: A Scope object
/// * buffer_size: The number of output elements to buffer in an iterator over
/// this dataset. Compare with the `min_after_dequeue` attr when creating a
/// `RandomShuffleQueue`.
/// * seed: A scalar seed for the random number generator. If either seed or
/// seed2 is set to be non-zero, the random number generator is seeded
/// by the given seed.  Otherwise, a random seed is used.
/// * seed2: A second scalar seed to avoid seed collision.
///
/// Returns:
/// * `Output`: The handle tensor.
class ShuffleDataset {
 public:
  ShuffleDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
               input_dataset, ::tensorflow::Input buffer_size,
               ::tensorflow::Input seed, ::tensorflow::Input seed2, const
               DataTypeSlice& output_types, const
               gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that skips `count` elements from the `input_dataset`.
///
/// Arguments:
/// * scope: A Scope object
/// * count: A scalar representing the number of elements from the `input_dataset`
/// that should be skipped.  If count is -1, skips everything.
///
/// Returns:
/// * `Output`: The handle tensor.
class SkipDataset {
 public:
  SkipDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
            input_dataset, ::tensorflow::Input count, const DataTypeSlice&
            output_types, const gtl::ArraySlice<PartialTensorShape>&
            output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that splits a SparseTensor into elements row-wise.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * `Output`: The handle tensor.
class SparseTensorSliceDataset {
 public:
  SparseTensorSliceDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                         indices, ::tensorflow::Input values,
                         ::tensorflow::Input dense_shape);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that emits the records from one or more TFRecord files.
///
/// Arguments:
/// * scope: A Scope object
/// * filenames: A scalar or vector containing the name(s) of the file(s) to be
/// read.
/// * compression_type: A scalar containing either (i) the empty string (no
/// compression), (ii) "ZLIB", or (iii) "GZIP".
///
/// Returns:
/// * `Output`: The handle tensor.
class TFRecordDataset {
 public:
  TFRecordDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                filenames, ::tensorflow::Input compression_type);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that contains `count` elements from the `input_dataset`.
///
/// Arguments:
/// * scope: A Scope object
/// * count: A scalar representing the number of elements from the `input_dataset`
/// that should be taken. A value of `-1` indicates that all of `input_dataset`
/// is taken.
///
/// Returns:
/// * `Output`: The handle tensor.
class TakeDataset {
 public:
  TakeDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
            input_dataset, ::tensorflow::Input count, const DataTypeSlice&
            output_types, const gtl::ArraySlice<PartialTensorShape>&
            output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that emits `components` as a tuple of tensors once.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * `Output`: The handle tensor.
class TensorDataset {
 public:
  TensorDataset(const ::tensorflow::Scope& scope, ::tensorflow::InputList
              components, const gtl::ArraySlice<PartialTensorShape>&
              output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that emits each dim-0 slice of `components` once.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * `Output`: The handle tensor.
class TensorSliceDataset {
 public:
  TensorSliceDataset(const ::tensorflow::Scope& scope, ::tensorflow::InputList
                   components, const gtl::ArraySlice<PartialTensorShape>&
                   output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that emits the lines of one or more text files.
///
/// Arguments:
/// * scope: A Scope object
/// * filenames: A scalar or a vector containing the name(s) of the file(s) to be
/// read.
///
/// Returns:
/// * `Output`: The handle tensor.
class TextLineDataset {
 public:
  TextLineDataset(const ::tensorflow::Scope& scope, ::tensorflow::Input
                filenames);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// Creates a dataset that zips together `input_datasets`.
///
/// Arguments:
/// * scope: A Scope object
///
/// Returns:
/// * `Output`: The handle tensor.
class ZipDataset {
 public:
  ZipDataset(const ::tensorflow::Scope& scope, ::tensorflow::InputList
           input_datasets, const DataTypeSlice& output_types, const
           gtl::ArraySlice<PartialTensorShape>& output_shapes);
  operator ::tensorflow::Output() const { return handle; }
  operator ::tensorflow::Input() const { return handle; }
  ::tensorflow::Node* node() const { return handle.node(); }

  ::tensorflow::Output handle;
};

/// @}

}  // namespace ops
}  // namespace tensorflow

#endif  // F__TENSORFLOW_R____TENSORFLOW_R____TENSORFLOW_CONTRIB_CMAKE_BUILD_TENSORFLOW_CC_OPS_DATASET_OPS_H_
