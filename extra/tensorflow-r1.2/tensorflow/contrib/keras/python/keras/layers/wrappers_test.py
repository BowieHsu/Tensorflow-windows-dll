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
"""Tests for layer wrappers."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from tensorflow.contrib.keras.python import keras
from tensorflow.python.platform import test


class TimeDistributedTest(test.TestCase):

  def test_timedistributed_dense(self):
    # first, test with Dense layer
    with self.test_session():
      model = keras.models.Sequential()
      model.add(
          keras.layers.TimeDistributed(
              keras.layers.Dense(2), input_shape=(3, 4)))
      model.add(keras.layers.Activation('relu'))
      model.compile(optimizer='rmsprop', loss='mse')
      model.fit(
          np.random.random((10, 3, 4)),
          np.random.random((10, 3, 2)),
          epochs=1,
          batch_size=10)

      # test config
      model.get_config()

  def test_timedistributed_conv2d(self):
    # test with Conv2D
    with self.test_session():
      model = keras.models.Sequential()
      model.add(
          keras.layers.TimeDistributed(
              keras.layers.Conv2D(5, (2, 2), padding='same'),
              input_shape=(2, 4, 4, 3)))
      model.add(keras.layers.Activation('relu'))
      model.compile(optimizer='rmsprop', loss='mse')
      model.train_on_batch(
          np.random.random((1, 2, 4, 4, 3)), np.random.random((1, 2, 4, 4, 5)))

      model = keras.models.model_from_json(model.to_json())
      model.summary()

  def test_timedistributed_stacked(self):
    # test stacked layers
    with self.test_session():
      model = keras.models.Sequential()
      model.add(
          keras.layers.TimeDistributed(
              keras.layers.Dense(2), input_shape=(3, 4)))
      model.add(keras.layers.TimeDistributed(keras.layers.Dense(3)))
      model.add(keras.layers.Activation('relu'))
      model.compile(optimizer='rmsprop', loss='mse')

      model.fit(
          np.random.random((10, 3, 4)),
          np.random.random((10, 3, 3)),
          epochs=1,
          batch_size=10)

  def test_timedistributed_sequential(self):
    # test wrapping Sequential model
    with self.test_session():
      model = keras.models.Sequential()
      model.add(keras.layers.Dense(3, input_dim=2))
      outer_model = keras.models.Sequential()
      outer_model.add(keras.layers.TimeDistributed(model, input_shape=(3, 2)))
      outer_model.compile(optimizer='rmsprop', loss='mse')
      outer_model.fit(
          np.random.random((10, 3, 2)),
          np.random.random((10, 3, 3)),
          epochs=1,
          batch_size=10)

      # test with functional API
      x = keras.layers.Input(shape=(3, 2))
      y = keras.layers.TimeDistributed(model)(x)
      outer_model = keras.models.Model(x, y)
      outer_model.compile(optimizer='rmsprop', loss='mse')
      outer_model.fit(
          np.random.random((10, 3, 2)),
          np.random.random((10, 3, 3)),
          epochs=1,
          batch_size=10)

  def test_regularizers(self):
    with self.test_session():
      model = keras.models.Sequential()
      model.add(
          keras.layers.TimeDistributed(
              keras.layers.Dense(2, kernel_regularizer='l1'),
              input_shape=(3, 4)))
      model.add(keras.layers.Activation('relu'))
      model.compile(optimizer='rmsprop', loss='mse')
      self.assertEqual(len(model.losses), 1)


class BidirectionalTest(test.TestCase):

  def test_bidirectional(self):
    rnn = keras.layers.SimpleRNN
    samples = 2
    dim = 2
    timesteps = 2
    output_dim = 2
    with self.test_session():
      for mode in ['sum', 'concat']:
        x = np.random.random((samples, timesteps, dim))
        target_dim = 2 * output_dim if mode == 'concat' else output_dim
        y = np.random.random((samples, target_dim))

        # test with Sequential model
        model = keras.models.Sequential()
        model.add(
            keras.layers.Bidirectional(
                rnn(output_dim), merge_mode=mode, input_shape=(timesteps, dim)))
        model.compile(loss='mse', optimizer='sgd')
        model.fit(x, y, epochs=1, batch_size=1)

        # test config
        model.get_config()
        model = keras.models.model_from_json(model.to_json())
        model.summary()

  def test_bidirectional_stacked(self):
    # test stacked bidirectional layers
    rnn = keras.layers.SimpleRNN
    samples = 2
    dim = 2
    timesteps = 2
    output_dim = 2
    mode = 'sum'

    with self.test_session():
      x = np.random.random((samples, timesteps, dim))
      target_dim = 2 * output_dim if mode == 'concat' else output_dim
      y = np.random.random((samples, target_dim))

      model = keras.models.Sequential()
      model.add(
          keras.layers.Bidirectional(
              rnn(output_dim, return_sequences=True),
              merge_mode=mode,
              input_shape=(timesteps, dim)))
      model.add(keras.layers.Bidirectional(rnn(output_dim), merge_mode=mode))
      model.compile(loss='mse', optimizer='sgd')
      model.fit(x, y, epochs=1, batch_size=1)

      # test with functional API
      inputs = keras.layers.Input((timesteps, dim))
      output = keras.layers.Bidirectional(
          rnn(output_dim), merge_mode=mode)(inputs)
      model = keras.models.Model(inputs, output)
      model.compile(loss='mse', optimizer='sgd')
      model.fit(x, y, epochs=1, batch_size=1)

  def test_bidirectional_statefulness(self):
    # Bidirectional and stateful
    rnn = keras.layers.SimpleRNN
    samples = 2
    dim = 2
    timesteps = 2
    output_dim = 2
    mode = 'sum'

    with self.test_session():
      x = np.random.random((samples, timesteps, dim))
      target_dim = 2 * output_dim if mode == 'concat' else output_dim
      y = np.random.random((samples, target_dim))

      inputs = keras.layers.Input(batch_shape=(1, timesteps, dim))
      output = keras.layers.Bidirectional(
          rnn(output_dim, stateful=True), merge_mode=mode)(inputs)
      model = keras.models.Model(inputs, output)
      model.compile(loss='mse', optimizer='sgd')
      model.fit(x, y, epochs=1, batch_size=1)


if __name__ == '__main__':
  test.main()
