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
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import data_flow_ops
from tensorflow.python.ops import lookup_ops
from tensorflow.python.ops import math_ops
from tensorflow.python.ops import random_ops
from tensorflow.python.ops import string_ops
from tensorflow.python.ops import variable_scope
from tensorflow.python.platform import test


class MapDatasetTest(test.TestCase):

  def _buildMapDataset(self, components, count):
    def _map_fn(x, y, z):
      return math_ops.square(x), math_ops.square(y), math_ops.square(z)
    return (dataset_ops.Dataset.from_tensor_slices(components).map(_map_fn)
            .repeat(count))

  def testMapDataset(self):
    """Test an dataset that maps a TF function across its input elements."""
    # The pipeline is TensorSliceDataset -> MapDataset(square_3) ->
    # RepeatDataset(count).
    components = [np.arange(7),
                  np.array([[1, 2, 3]]) * np.arange(7)[:, np.newaxis],
                  np.array(37.0) * np.arange(7)]
    count = array_ops.placeholder(dtypes.int64, shape=[])

    dataset = self._buildMapDataset(components, count)
    iterator = dataset.make_initializable_iterator()
    init_op = iterator.initializer
    get_next = iterator.get_next()

    self.assertEqual([c.shape[1:] for c in components],
                     [t.shape for t in get_next])

    with self.test_session() as sess:
      # Test single-threaded access to the iterator.
      sess.run(init_op, feed_dict={count: 14})
      for _ in range(14):
        for i in range(7):
          result = sess.run(get_next)
          for component, result_component in zip(components, result):
            self.assertAllEqual(component[i]**2, result_component)
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test multi-threaded access to the same iterator.
      sess.run(init_op, feed_dict={count: 18})
      results = []
      def iterator_thread():
        while True:
          try:
            results.append(sess.run(get_next))
          except errors.OutOfRangeError:
            return
      threads = [self.checkedThread(target=iterator_thread) for _ in range(8)]
      for t in threads:
        t.start()
      for t in threads:
        t.join()

      # `results` will contain the same elements components**2
      # repeated 18 times, but in a non-deterministic order. Sort the
      # results, and assert that each element of components**2 is
      # produced 18 times.
      results.sort(key=lambda x: x[0])
      for i in range(7):
        for j in range(18):
          for component, result_component in zip(components,
                                                 results[i * 18 + j]):
            self.assertAllEqual(component[i]**2, result_component)

  def _buildParallelMapDataset(self, components, count, num_threads,
                               output_buffer_size):
    def _map_fn(x, y, z):
      return math_ops.square(x), math_ops.square(y), math_ops.square(z)
    return (dataset_ops.Dataset.from_tensor_slices(components).map(
        _map_fn, num_threads=num_threads, output_buffer_size=output_buffer_size)
            .repeat(count))

  def testParallelMapDataset(self):
    """Test an dataset that maps a TF function across its input elements."""
    # The pipeline is TensorSliceDataset -> ParallelMapDataset(square_3) ->
    # RepeatDataset(count).
    components = [np.arange(7),
                  np.array([[1, 2, 3]]) * np.arange(7)[:, np.newaxis],
                  np.array(37.0) * np.arange(7)]
    count = array_ops.placeholder(dtypes.int64, shape=[])
    num_threads = array_ops.placeholder(dtypes.int32, shape=[])
    output_buffer_size = array_ops.placeholder(dtypes.int64, shape=[])

    dataset = self._buildParallelMapDataset(components, count, num_threads,
                                            output_buffer_size)
    iterator = dataset.make_initializable_iterator()
    init_op = iterator.initializer
    get_next = iterator.get_next()

    self.assertEqual([c.shape[1:] for c in components],
                     [t.shape for t in get_next])

    with self.test_session() as sess:
      def do_test(num_threads_val, output_buffer_size_val):
        # Test single-threaded access to the iterator.
        sess.run(init_op, feed_dict={
            count: 14,
            num_threads: num_threads_val,
            output_buffer_size: output_buffer_size_val})
        for _ in range(14):
          for i in range(7):
            result = sess.run(get_next)
            for component, result_component in zip(components, result):
              self.assertAllEqual(component[i]**2, result_component)
        with self.assertRaises(errors.OutOfRangeError):
          sess.run(get_next)

        # Test multi-threaded access to the same iterator.
        sess.run(init_op, feed_dict={
            count: 18,
            num_threads: num_threads_val,
            output_buffer_size: output_buffer_size_val})
        results = []
        def iterator_thread():
          while True:
            try:
              results.append(sess.run(get_next))
            except errors.OutOfRangeError:
              return
        threads = [self.checkedThread(target=iterator_thread)
                   for _ in range(64)]
        for t in threads:
          t.start()
        for t in threads:
          t.join()

        # `results` will contain the same elements components**2
        # repeated 18 times, but in a non-deterministic order. Sort the
        # results, and assert that each element of components**2 is
        # produced 18 times.
        results.sort(key=lambda x: x[0])
        for i in range(7):
          for j in range(18):
            for component, result_component in zip(components,
                                                   results[i * 18 + j]):
              self.assertAllEqual(component[i]**2, result_component)

      for num_threads_val, output_buffer_size_val in [
          (1, 1), (1, 2), (2, 2), (2, 4), (8, 8), (8, 16)]:
        do_test(num_threads_val, output_buffer_size_val)

  def _testDisposeParallelMapDataset(self, explicit_dispose):
    # The pipeline is TensorSliceDataset -> MapDataset(square_3) ->
    # RepeatDataset(1000).
    components = [np.arange(1000),
                  np.array([[1, 2, 3]]) * np.arange(1000)[:, np.newaxis],
                  np.array(37.0) * np.arange(1000)]

    dataset = self._buildParallelMapDataset(components, 1000, 100, 100)
    iterator = dataset.make_initializable_iterator()
    init_op = iterator.initializer
    get_next = iterator.get_next()
    if explicit_dispose:
      dispose_op = iterator.dispose_op()

    with self.test_session() as sess:
      sess.run(init_op)
      for _ in range(3):
        sess.run(get_next)
      if explicit_dispose:
        sess.run(dispose_op)

  def testExplicitDisposeParallelMapDataset(self):
    self._testDisposeParallelMapDataset(True)

  def testImplicitDisposeParallelMapDataset(self):
    self._testDisposeParallelMapDataset(False)

  def testParallelMapError(self):
    components = [np.array([1., 2., 3., np.nan, 5.]).astype(np.float32)]

    dataset = (dataset_ops.Dataset.from_tensor_slices(components)
               .map(lambda x: array_ops.check_numerics(x, "message")))
    iterator = dataset.make_initializable_iterator()
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)
      for _ in range(3):
        sess.run(get_next)
      # The 4th element is NaN, so `array_ops.check_numerics()` should fail.
      with self.assertRaises(errors.InvalidArgumentError):
        sess.run(get_next)
      sess.run(get_next)
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

  def testCaptureHashTable(self):
    # NOTE(mrry): We must use the V2 variants of `HashTable`
    # etc. because these produce a `tf.resource`-typed output that is
    # compatible with the in-graph function implementation.
    default_val = -1
    keys = constant_op.constant(["brain", "salad", "surgery"])
    values = constant_op.constant([0, 1, 2], dtypes.int64)
    table = lookup_ops.HashTable(
        lookup_ops.KeyValueTensorInitializer(keys, values), default_val)

    input_sentences = dataset_ops.Dataset.from_tensor_slices(
        constant_op.constant([
            "brain brain tank salad surgery",
            "surgery brain",
        ]))

    iterator = (input_sentences
                .map(lambda x: string_ops.string_split([x]).values)
                .map(table.lookup)
                .make_initializable_iterator())
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(table.init)
      sess.run(init_op)

      print(sess.run(get_next))
      print(sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

  def testCaptureQueue(self):
    elements = np.random.randint(100, size=[200])
    queue = data_flow_ops.FIFOQueue(200, dtypes.int64, shapes=[])
    enqueue_op = queue.enqueue_many(elements)
    close_op = queue.close()
    iterator = (dataset_ops.Dataset.from_tensors(0).repeat(-1)
                .map(lambda _: queue.dequeue()).make_initializable_iterator())
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(enqueue_op)
      sess.run(close_op)
      sess.run(init_op)
      for element in elements:
        self.assertEqual(element, sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

  def testCaptureVariable(self):
    counter_var = variable_scope.get_variable(
        "counter", (), dtypes.int32, use_resource=True)
    iterator = (dataset_ops.Dataset.from_tensors(0).repeat(10)
                .map(lambda _: counter_var.assign_add(1))
                .make_initializable_iterator())
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(counter_var.initializer)
      sess.run(init_op)
      for i in range(10):
        self.assertEqual(i, sess.run(counter_var))
        self.assertEqual(i + 1, sess.run(get_next))
      self.assertEqual(10, sess.run(counter_var))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)
      self.assertEqual(10, sess.run(counter_var))

  def testCaptureUninitializedVariableError(self):
    counter_var = variable_scope.get_variable(
        "counter", (), dtypes.int32, use_resource=True)
    iterator = (dataset_ops.Dataset.from_tensors(0).repeat(10)
                .map(lambda _: counter_var.assign_add(1))
                .make_initializable_iterator())
    init_op = iterator.initializer

    with self.test_session() as sess:
      with self.assertRaisesRegexp(errors.FailedPreconditionError,
                                   "Failed to capture resource"):
        sess.run(init_op)

  def testSeededStatefulOperatorIsProperlyStateful(self):
    iterator = (dataset_ops.Dataset.from_tensors(0).repeat(10)
                .map(lambda _: random_ops.random_uniform((), seed=11)).batch(2)
                .make_initializable_iterator())
    init_op = iterator.initializer
    get_next = iterator.get_next()

    with self.test_session() as sess:
      sess.run(init_op)
      random_values = []
      with self.assertRaises(errors.OutOfRangeError):
        while True:
          random_values.extend(sess.run(get_next))
      self.assertEqual(10, len(random_values))
      self.assertGreater(np.abs(np.diff(random_values)).max(), 1e-6)
      sess.run(init_op)
      random_values_2 = []
      with self.assertRaises(errors.OutOfRangeError):
        while True:
          random_values_2.extend(sess.run(get_next))

      # Randomness is repeatable given same seed
      self.assertAllClose(random_values, random_values_2)

if __name__ == "__main__":
  test.main()
