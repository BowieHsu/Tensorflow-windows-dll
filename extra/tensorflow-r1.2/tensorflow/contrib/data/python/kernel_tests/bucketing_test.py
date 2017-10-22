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

import numpy as np

from tensorflow.contrib.data.python.ops import dataset_ops
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import errors
from tensorflow.python.framework import ops
from tensorflow.python.framework import tensor_shape
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import math_ops
from tensorflow.python.ops import string_ops
from tensorflow.python.platform import test


class BucketingTest(test.TestCase):

  def testSimple(self):
    components = np.random.randint(100, size=(200,)).astype(np.int64)
    iterator = dataset_ops.Iterator.from_dataset(
        dataset_ops.Dataset.from_tensor_slices(components).map(lambda x: x * x)
        .group_by_window(lambda x: x % 2, lambda _, xs: xs.batch(4), 4))
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)
      counts = []
      with self.assertRaises(errors.OutOfRangeError):
        while True:
          result = sess.run(get_next)
          self.assertTrue(
              all(x % 2 == 0 for x in result) or all(x % 2 == 1)
              for x in result)
          counts.append(result.shape[0])

      self.assertEqual(len(components), sum(counts))
      num_full_batches = len([c for c in counts if c == 4])
      self.assertGreaterEqual(num_full_batches, 23)
      self.assertTrue(all(c == 4 for c in counts[:num_full_batches]))

  def testImmediateOutput(self):
    components = np.array(
        [0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 0, 0, 2, 2, 0, 0], dtype=np.int64)
    iterator = dataset_ops.Iterator.from_dataset(
        dataset_ops.Dataset.from_tensor_slices(components).repeat(-1)
        .group_by_window(lambda x: x % 3, lambda _, xs: xs.batch(4), 4))
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)
      # The input is infinite, so this test demonstrates that:
      # 1. We produce output without having to consume the entire input,
      # 2. Different buckets can produce output at different rates, and
      # 3. For deterministic input, the output is deterministic.
      for _ in range(3):
        self.assertAllEqual([0, 0, 0, 0], sess.run(get_next))
        self.assertAllEqual([1, 1, 1, 1], sess.run(get_next))
        self.assertAllEqual([2, 2, 2, 2], sess.run(get_next))
        self.assertAllEqual([0, 0, 0, 0], sess.run(get_next))

  def testSmallGroups(self):
    components = np.array([0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0], dtype=np.int64)
    iterator = dataset_ops.Iterator.from_dataset(
        dataset_ops.Dataset.from_tensor_slices(components)
        .group_by_window(lambda x: x % 2, lambda _, xs: xs.batch(4), 4))
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)
      self.assertAllEqual([0, 0, 0, 0], sess.run(get_next))
      self.assertAllEqual([1, 1, 1, 1], sess.run(get_next))
      # The small outputs at the end are deterministically produced in key
      # order.
      self.assertAllEqual([0, 0, 0], sess.run(get_next))
      self.assertAllEqual([1], sess.run(get_next))

  def testReduceFuncError(self):
    components = np.random.randint(100, size=(200,)).astype(np.int64)

    def reduce_func(_, xs):
      # Introduce an incorrect padded shape that cannot (currently) be
      # detected at graph construction time.
      return xs.padded_batch(
          4,
          padded_shapes=(tensor_shape.TensorShape([]),
                         constant_op.constant([5], dtype=dtypes.int64) * -1))

    iterator = dataset_ops.Iterator.from_dataset(
        dataset_ops.Dataset.from_tensor_slices(components)
        .map(lambda x: (x, ops.convert_to_tensor([x * x])))
        .group_by_window(lambda x, _: x % 2, reduce_func, 32))
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)
      with self.assertRaises(errors.InvalidArgumentError):
        sess.run(get_next)

  def testConsumeWindowDatasetMoreThanOnce(self):
    components = np.random.randint(50, size=(200,)).astype(np.int64)

    def reduce_func(key, window):
      # Apply two different kinds of padding to the input: tight
      # padding, and quantized (to a multiple of 10) padding.
      return dataset_ops.Dataset.zip((window.padded_batch(
          4,
          padded_shapes=tensor_shape.TensorShape([None])), window.padded_batch(
              4, padded_shapes=ops.convert_to_tensor([(key + 1) * 10])),))

    iterator = dataset_ops.Iterator.from_dataset(
        dataset_ops.Dataset.from_tensor_slices(components)
        .map(lambda x: array_ops.fill([math_ops.cast(x, dtypes.int32)], x))
        .group_by_window(
            lambda x: math_ops.cast(array_ops.shape(x)[0] // 10, dtypes.int64),
            reduce_func, 4))
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)
      counts = []
      with self.assertRaises(errors.OutOfRangeError):
        while True:
          tight_result, multiple_of_10_result = sess.run(get_next)
          self.assertEqual(0, multiple_of_10_result.shape[1] % 10)
          self.assertAllEqual(tight_result,
                              multiple_of_10_result[:, :tight_result.shape[1]])
          counts.append(tight_result.shape[0])
      self.assertEqual(len(components), sum(counts))


# NOTE(mrry): These tests are based on the tests in
# bucket_ops_test.py. Currently, different batch sizes for each key
# are not supported, although this would be possible to add to
# `Dataset.group_by_window()`.
class BucketTest(test.TestCase):

  def _dynamicPad(self, bucket, window, window_size):
    # TODO(mrry): To match `tf.contrib.training.bucket()`, implement a
    # generic form of padded_batch that pads every component
    # dynamically and does not rely on static shape information about
    # the arguments.
    return dataset_ops.Dataset.zip(
        (dataset_ops.Dataset.from_tensors(bucket), window.padded_batch(
            32, (tensor_shape.TensorShape([]), tensor_shape.TensorShape([None]),
                 tensor_shape.TensorShape([3])))))

  def testSingleBucket(self):
    def _map_fn(v):
      return (v, array_ops.fill([v], v),
              array_ops.fill([3], string_ops.as_string(v)))

    input_dataset = (
        dataset_ops.Dataset.from_tensor_slices(math_ops.range(32)).map(_map_fn))

    bucketed_dataset = input_dataset.group_by_window(
        lambda x, y, z: 0, lambda k, bucket: self._dynamicPad(k, bucket, 32),
        32)

    iterator = dataset_ops.Iterator.from_dataset(bucketed_dataset)
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)

      which_bucket, bucketed_values = sess.run(get_next)

      self.assertEqual(0, which_bucket)

      expected_scalar_int = np.arange(32, dtype=np.int64)
      expected_unk_int64 = np.zeros((32, 31)).astype(np.int64)
      for i in range(32):
        expected_unk_int64[i, :i] = i
      expected_vec3_str = np.vstack(3 * [np.arange(32).astype(bytes)]).T

      self.assertAllEqual(expected_scalar_int, bucketed_values[0])
      self.assertAllEqual(expected_unk_int64, bucketed_values[1])
      self.assertAllEqual(expected_vec3_str, bucketed_values[2])

  def testEvenOddBuckets(self):
    def _map_fn(v):
      return (v, array_ops.fill([v], v),
              array_ops.fill([3], string_ops.as_string(v)))

    input_dataset = (
        dataset_ops.Dataset.from_tensor_slices(math_ops.range(64)).map(_map_fn))

    bucketed_dataset = input_dataset.group_by_window(
        lambda x, y, z: math_ops.cast(x % 2, dtypes.int64),
        lambda k, bucket: self._dynamicPad(k, bucket, 32), 32)

    iterator = dataset_ops.Iterator.from_dataset(bucketed_dataset)
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)

      # Get two minibatches (one containing even values, one containing odds)
      which_bucket_even, bucketed_values_even = sess.run(get_next)
      which_bucket_odd, bucketed_values_odd = sess.run(get_next)

      # Count number of bucket_tensors.
      self.assertEqual(3, len(bucketed_values_even))
      self.assertEqual(3, len(bucketed_values_odd))

      # Ensure bucket 0 was used for all minibatch entries.
      self.assertAllEqual(0, which_bucket_even)
      self.assertAllEqual(1, which_bucket_odd)

      # Test the first bucket outputted, the events starting at 0
      expected_scalar_int = np.arange(0, 32 * 2, 2, dtype=np.int64)
      expected_unk_int64 = np.zeros((32, 31 * 2)).astype(np.int64)
      for i in range(0, 32):
        expected_unk_int64[i, :2 * i] = 2 * i
        expected_vec3_str = np.vstack(
            3 * [np.arange(0, 32 * 2, 2).astype(bytes)]).T

      self.assertAllEqual(expected_scalar_int, bucketed_values_even[0])
      self.assertAllEqual(expected_unk_int64, bucketed_values_even[1])
      self.assertAllEqual(expected_vec3_str, bucketed_values_even[2])

      # Test the second bucket outputted, the odds starting at 1
      expected_scalar_int = np.arange(1, 32 * 2 + 1, 2, dtype=np.int64)
      expected_unk_int64 = np.zeros((32, 31 * 2 + 1)).astype(np.int64)
      for i in range(0, 32):
        expected_unk_int64[i, :2 * i + 1] = 2 * i + 1
        expected_vec3_str = np.vstack(
            3 * [np.arange(1, 32 * 2 + 1, 2).astype(bytes)]).T

      self.assertAllEqual(expected_scalar_int, bucketed_values_odd[0])
      self.assertAllEqual(expected_unk_int64, bucketed_values_odd[1])
      self.assertAllEqual(expected_vec3_str, bucketed_values_odd[2])

  def testEvenOddBucketsFilterOutAllOdd(self):
    def _map_fn(v):
      return (v, array_ops.fill([v], v),
              array_ops.fill([3], string_ops.as_string(v)))

    input_dataset = (
        dataset_ops.Dataset.from_tensor_slices(math_ops.range(128)).map(_map_fn)
        .filter(lambda x, y, z: math_ops.equal(x % 2, 0)))

    bucketed_dataset = input_dataset.group_by_window(
        lambda x, y, z: math_ops.cast(x % 2, dtypes.int64),
        lambda k, bucket: self._dynamicPad(k, bucket, 32), 32)

    iterator = dataset_ops.Iterator.from_dataset(bucketed_dataset)
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)

      # Get two minibatches ([0, 2, ...] and [64, 66, ...])
      which_bucket0, bucketed_values_even0 = sess.run(get_next)
      which_bucket1, bucketed_values_even1 = sess.run(get_next)

      # Ensure that bucket 1 was completely filtered out
      self.assertAllEqual(0, which_bucket0)
      self.assertAllEqual(0, which_bucket1)
      self.assertAllEqual(
          np.arange(0, 64, 2, dtype=np.int64), bucketed_values_even0[0])
      self.assertAllEqual(
          np.arange(64, 128, 2, dtype=np.int64), bucketed_values_even1[0])


if __name__ == "__main__":
  test.main()
