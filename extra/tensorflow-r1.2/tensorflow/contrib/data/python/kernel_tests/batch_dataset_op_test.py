# Copyright 2017 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================
"""Tests for the experimental input pipeline ops."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import math

import numpy as np

from tensorflow.contrib.data.python.ops import dataset_ops
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import errors
from tensorflow.python.framework import sparse_tensor
from tensorflow.python.framework import tensor_shape
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import math_ops
from tensorflow.python.ops import string_ops
from tensorflow.python.platform import test
from tensorflow.python.util import compat


class BatchDatasetTest(test.TestCase):

  def testBatchDataset(self):
    """Test an dataset that maps a TF function across its input elements."""
    # The pipeline is TensorSliceDataset -> MapDataset(square_3) ->
    # RepeatDataset(count) -> BatchDataset(batch_size).
    components = [np.arange(7),
                  np.array([[1, 2, 3]]) * np.arange(7)[:, np.newaxis],
                  np.array(37.0) * np.arange(7)]

    count = array_ops.placeholder(dtypes.int64, shape=[])
    batch_size = array_ops.placeholder(dtypes.int64, shape=[])

    def _map_fn(x, y, z):
      return math_ops.square(x), math_ops.square(y), math_ops.square(z)

    iterator = (dataset_ops.Dataset.from_tensor_slices(components).map(_map_fn)
                .repeat(count).batch(batch_size).make_initializable_iterator())
    init_op = iterator.initializer
    get_next = iterator.get_next()

    self.assertEqual([[None] + list(c.shape[1:]) for c in components],
                     [t.shape.as_list() for t in get_next])

    with self.test_session() as sess:
      # Batch of a finite input, where the batch_size divides the
      # total number of elements.
      sess.run(init_op, feed_dict={count: 28, batch_size: 14})
      num_batches = (28 * 7) // 14
      for i in range(num_batches):
        result = sess.run(get_next)
        for component, result_component in zip(components, result):
          for j in range(14):
            self.assertAllEqual(component[(i*14 + j) % 7]**2,
                                result_component[j])
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Batch of a finite input, where the batch_size does not
      # divide the total number of elements.
      sess.run(init_op, feed_dict={count: 14, batch_size: 8})

      # We expect (num_batches - 1) full-sized batches.
      num_batches = int(math.ceil((14 * 7) / 8))
      for i in range(num_batches - 1):
        result = sess.run(get_next)
        for component, result_component in zip(components, result):
          for j in range(8):
            self.assertAllEqual(component[(i*8 + j) % 7]**2,
                                result_component[j])
      result = sess.run(get_next)
      for component, result_component in zip(components, result):
        for j in range((14 * 7) % 8):
          self.assertAllEqual(component[((num_batches - 1)*8 + j) % 7]**2,
                              result_component[j])
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Batch of an empty input should fail straight away.
      sess.run(init_op, feed_dict={count: 0, batch_size: 8})
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Empty batch should be an initialization time error.
      with self.assertRaises(errors.InvalidArgumentError):
        sess.run(init_op, feed_dict={count: 14, batch_size: 0})

  def testPaddedBatchDataset(self):
    seq_lens = array_ops.placeholder(dtypes.int32, shape=[None])
    padded_shape = array_ops.placeholder(dtypes.int64, shape=[1])

    iterator = (dataset_ops.Dataset.from_tensor_slices(seq_lens)
                .map(lambda x: array_ops.fill([x], x)).padded_batch(
                    4,
                    padded_shapes=padded_shape).make_initializable_iterator())

    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      # Test with random sequence lengths, and max padding.
      random_seq_lens = np.random.randint(20, size=(32,)).astype(np.int32)
      sess.run(init_op, feed_dict={padded_shape: [-1],
                                   seq_lens: random_seq_lens})
      for i in range(8):
        result = sess.run(get_next)
        padded_len = np.max(result)
        self.assertEqual((4, padded_len), result.shape)
        for j in range(4):
          seq_len = random_seq_lens[(i*4)+j]
          self.assertAllEqual(result[j, :seq_len], [seq_len] * seq_len)
          self.assertAllEqual(result[j, seq_len:], [0] * (padded_len - seq_len))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test with random sequence lengths, and constant padding.
      sess.run(init_op, feed_dict={padded_shape: [25],
                                   seq_lens: random_seq_lens})
      for i in range(8):
        result = sess.run(get_next)
        self.assertEqual((4, 25), result.shape)
        for j in range(4):
          seq_len = random_seq_lens[(i*4)+j]
          self.assertAllEqual(result[j, :seq_len], [seq_len] * seq_len)
          self.assertAllEqual(result[j, seq_len:], [0] * (25 - seq_len))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test correct handling of empty tensors.
      sess.run(init_op, feed_dict={padded_shape: [-1],
                                   seq_lens: [0, 0, 0, 0]})
      result = sess.run(get_next)
      self.assertAllEqual([[], [], [], []], result)
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test error handling with constant sequence lengths, and
      # too-short padding.
      sess.run(init_op, feed_dict={padded_shape: [5],
                                   seq_lens: [6, 5, 5, 5]})
      with self.assertRaises(errors.DataLossError):
        result = sess.run(get_next)

  def testPaddedBatchDatasetNonDefaultPadding(self):
    seq_lens = array_ops.placeholder(dtypes.int32, shape=[None])
    padded_shape = array_ops.placeholder(dtypes.int64, shape=[1])

    def fill_tuple(x):
      filled = array_ops.fill([x], x)
      return (filled, string_ops.as_string(filled))
    iterator = (dataset_ops.Dataset.from_tensor_slices(seq_lens).map(fill_tuple)
                .padded_batch(
                    4,
                    padded_shapes=(padded_shape, padded_shape),
                    padding_values=(-1, "<end>")).make_initializable_iterator())

    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      # Test with random sequence lengths, and max padding.
      random_seq_lens = np.random.randint(20, size=(32,)).astype(np.int32)
      sess.run(init_op, feed_dict={padded_shape: [-1],
                                   seq_lens: random_seq_lens})
      for i in range(8):
        result = sess.run(get_next)
        padded_len = np.max(result[0])
        self.assertEqual((4, padded_len), result[0].shape)
        self.assertEqual((4, padded_len), result[1].shape)
        for j in range(4):
          seq_len = random_seq_lens[(i*4)+j]
          self.assertAllEqual(result[0][j, :seq_len], [seq_len] * seq_len)
          self.assertAllEqual(result[0][j, seq_len:],
                              [-1] * (padded_len - seq_len))
          self.assertAllEqual(result[1][j, :seq_len],
                              [compat.as_bytes(str(seq_len))] * seq_len)
          self.assertAllEqual(result[1][j, seq_len:],
                              [b"<end>"] * (padded_len - seq_len))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

  def testPaddedBatchDatasetShapeSpecifications(self):
    int_placeholder = array_ops.placeholder(dtypes.int32)
    float_placeholder = array_ops.placeholder(dtypes.float32)
    string_placeholder = array_ops.placeholder(dtypes.string)
    input_dataset = dataset_ops.Dataset.from_tensors(
        (int_placeholder, float_placeholder, string_placeholder))

    # Test different ways of specifying the `padded_shapes` argument.
    dynamic_padding_from_tensor_shapes = input_dataset.padded_batch(
        32,
        padded_shapes=(tensor_shape.TensorShape([None]),
                       tensor_shape.TensorShape([None, None]),
                       tensor_shape.TensorShape([37])))
    dynamic_padding_from_lists = input_dataset.padded_batch(
        32, padded_shapes=([None], [None, None], [37]))
    dynamic_padding_from_lists_with_minus_one = input_dataset.padded_batch(
        32, padded_shapes=([-1], [-1, -1], [37]))
    dynamic_padding_from_tensors = input_dataset.padded_batch(
        32,
        padded_shapes=(constant_op.constant([-1], dtype=dtypes.int64),
                       constant_op.constant([-1, -1], dtype=dtypes.int64),
                       constant_op.constant([37], dtype=dtypes.int64)))

    for dataset in [dynamic_padding_from_tensor_shapes,
                    dynamic_padding_from_lists,
                    dynamic_padding_from_lists_with_minus_one,
                    dynamic_padding_from_tensors]:
      self.assertEqual([None, None], dataset.output_shapes[0].as_list())
      self.assertEqual([None, None, None], dataset.output_shapes[1].as_list())
      self.assertEqual([None, 37], dataset.output_shapes[2].as_list())

  def testDenseToSparseBatchDataset(self):
    components = np.random.randint(12, size=(100,)).astype(np.int32)
    iterator = (dataset_ops.Dataset.from_tensor_slices(components)
                .map(lambda x: array_ops.fill([x], x)).dense_to_sparse_batch(
                    4, [12]).make_initializable_iterator())
    init_op = iterator.initializer
    get_next = sparse_tensor.SparseTensor(*iterator.get_next())

    with self.test_session() as sess:
      sess.run(init_op)

      for start in range(0, len(components), 4):
        results = sess.run(get_next)
        self.assertAllEqual(
            [[i, j] for i, c in enumerate(components[start:start+4])
             for j in range(c)], results.indices)
        self.assertAllEqual(
            [c for c in components[start:start+4] for _ in range(c)],
            results.values)
        self.assertAllEqual(
            [min(4, len(components) - start), 12], results.dense_shape)

      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

  def testDenseToSparseBatchDatasetShapeErrors(self):
    input_tensor = array_ops.placeholder(dtypes.int32)
    iterator = (dataset_ops.Dataset.from_tensors(input_tensor)
                .dense_to_sparse_batch(4, [12]).make_initializable_iterator())
    init_op = iterator.initializer
    get_next = sparse_tensor.SparseTensor(*iterator.get_next())

    with self.test_session() as sess:
      # Initialize with an input tensor of incompatible rank.
      sess.run(init_op, feed_dict={input_tensor: [[1]]})
      with self.assertRaisesRegexp(errors.InvalidArgumentError,
                                   "incompatible with the row shape"):
        sess.run(get_next)

      # Initialize with an input tensor that is larger than `row_shape`.
      sess.run(init_op, feed_dict={input_tensor: range(13)})
      with self.assertRaisesRegexp(errors.DataLossError,
                                   "larger than the row shape"):
        sess.run(get_next)


if __name__ == "__main__":
  test.main()
