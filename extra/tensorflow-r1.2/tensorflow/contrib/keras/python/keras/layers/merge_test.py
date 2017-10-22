# Copyright 2016 The TensorFlow Authors. All Rights Reserved.
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
"""Tests for merge layers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from tensorflow.contrib.keras.python import keras
from tensorflow.python.platform import test


class MergeLayersTest(test.TestCase):

  def test_merge_add(self):
    with self.test_session():
      i1 = keras.layers.Input(shape=(4, 5))
      i2 = keras.layers.Input(shape=(4, 5))
      i3 = keras.layers.Input(shape=(4, 5))

      o = keras.layers.add([i1, i2, i3])
      self.assertListEqual(o.get_shape().as_list(), [None, 4, 5])
      model = keras.models.Model([i1, i2, i3], o)

      x1 = np.random.random((2, 4, 5))
      x2 = np.random.random((2, 4, 5))
      x3 = np.random.random((2, 4, 5))
      out = model.predict([x1, x2, x3])
      self.assertEqual(out.shape, (2, 4, 5))
      self.assertAllClose(out, x1 + x2 + x3, atol=1e-4)

      # test masking
      i1 = keras.layers.Input(shape=(4, 5))
      i2 = keras.layers.Input(shape=(4, 5))
      m1 = keras.layers.Masking()(i1)
      layer = keras.layers.Add()
      o = layer([m1, i2])
      self.assertListEqual(o.get_shape().as_list(), [None, 4, 5])
      mask = layer.output_mask
      self.assertListEqual(mask.get_shape().as_list(), [None, 4])

  def test_merge_elementwise_errors(self):
    i1 = keras.layers.Input(shape=(4, 5))
    i2 = keras.layers.Input(shape=(4, 6))
    with self.assertRaises(ValueError):
      keras.layers.add([i1, i2])
    with self.assertRaises(ValueError):
      keras.layers.add(i1)
    with self.assertRaises(ValueError):
      keras.layers.add([i1])

  def test_merge_multiply(self):
    with self.test_session():
      i1 = keras.layers.Input(shape=(4, 5))
      i2 = keras.layers.Input(shape=(4, 5))
      i3 = keras.layers.Input(shape=(4, 5))
      o = keras.layers.multiply([i1, i2, i3])
      self.assertListEqual(o.get_shape().as_list(), [None, 4, 5])
      model = keras.models.Model([i1, i2, i3], o)

      x1 = np.random.random((2, 4, 5))
      x2 = np.random.random((2, 4, 5))
      x3 = np.random.random((2, 4, 5))
      out = model.predict([x1, x2, x3])
      self.assertEqual(out.shape, (2, 4, 5))
      self.assertAllClose(out, x1 * x2 * x3, atol=1e-4)

  def test_merge_average(self):
    with self.test_session():
      i1 = keras.layers.Input(shape=(4, 5))
      i2 = keras.layers.Input(shape=(4, 5))
      o = keras.layers.average([i1, i2])
      self.assertListEqual(o.get_shape().as_list(), [None, 4, 5])
      model = keras.models.Model([i1, i2], o)

      x1 = np.random.random((2, 4, 5))
      x2 = np.random.random((2, 4, 5))
      out = model.predict([x1, x2])
      self.assertEqual(out.shape, (2, 4, 5))
      self.assertAllClose(out, 0.5 * (x1 + x2), atol=1e-4)

  def test_merge_maximum(self):
    with self.test_session():
      i1 = keras.layers.Input(shape=(4, 5))
      i2 = keras.layers.Input(shape=(4, 5))
      o = keras.layers.maximum([i1, i2])
      self.assertListEqual(o.get_shape().as_list(), [None, 4, 5])
      model = keras.models.Model([i1, i2], o)

      x1 = np.random.random((2, 4, 5))
      x2 = np.random.random((2, 4, 5))
      out = model.predict([x1, x2])
      self.assertEqual(out.shape, (2, 4, 5))
      self.assertAllClose(out, np.maximum(x1, x2), atol=1e-4)

  def test_merge_concatenate(self):
    with self.test_session():
      i1 = keras.layers.Input(shape=(4, 5))
      i2 = keras.layers.Input(shape=(4, 5))
      o = keras.layers.concatenate([i1, i2], axis=1)
      self.assertListEqual(o.get_shape().as_list(), [None, 8, 5])
      model = keras.models.Model([i1, i2], o)

      x1 = np.random.random((2, 4, 5))
      x2 = np.random.random((2, 4, 5))
      out = model.predict([x1, x2])
      self.assertEqual(out.shape, (2, 8, 5))
      self.assertAllClose(out, np.concatenate([x1, x2], axis=1), atol=1e-4)

  def test_concatenate_errors(self):
    i1 = keras.layers.Input(shape=(4, 5))
    i2 = keras.layers.Input(shape=(3, 5))
    with self.assertRaises(ValueError):
      keras.layers.concatenate([i1, i2], axis=-1)
    with self.assertRaises(ValueError):
      keras.layers.concatenate(i1, axis=-1)
    with self.assertRaises(ValueError):
      keras.layers.concatenate([i1], axis=-1)

  def test_merge_dot(self):
    with self.test_session():
      i1 = keras.layers.Input(shape=(4,))
      i2 = keras.layers.Input(shape=(4,))
      o = keras.layers.dot([i1, i2], axes=1)
      self.assertListEqual(o.get_shape().as_list(), [None, 1])
      model = keras.models.Model([i1, i2], o)

      x1 = np.random.random((2, 4))
      x2 = np.random.random((2, 4))
      out = model.predict([x1, x2])
      self.assertEqual(out.shape, (2, 1))
      expected = np.zeros((2, 1))
      expected[0, 0] = np.dot(x1[0], x2[0])
      expected[1, 0] = np.dot(x1[1], x2[1])
      self.assertAllClose(out, expected, atol=1e-4)

      # Test with negative tuple of axes.
      o = keras.layers.dot([i1, i2], axes=(-1, -1))
      self.assertListEqual(o.get_shape().as_list(), [None, 1])
      model = keras.models.Model([i1, i2], o)
      out = model.predict([x1, x2])
      self.assertEqual(out.shape, (2, 1))
      self.assertAllClose(out, expected, atol=1e-4)

      # test _compute_output_shape
      layer = keras.layers.Dot(axes=-1)
      self.assertEqual(layer._compute_output_shape([(4, 5), (4, 5)]), (4, 1))

  def test_dot_errors(self):
    i1 = keras.layers.Input(shape=(4, 5))
    i2 = keras.layers.Input(shape=(4, 6))
    i3 = keras.layers.Input(shape=(4, 6))
    with self.assertRaises(ValueError):
      keras.layers.dot([i1, i2], axes=-1)
    with self.assertRaises(ValueError):
      keras.layers.dot(i1, axes=-1)
    with self.assertRaises(ValueError):
      keras.layers.dot([i1], axes=-1)
    with self.assertRaises(ValueError):
      keras.layers.dot([i1, i2, i3], axes=-1)


if __name__ == '__main__':
  test.main()
