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
"""Tests for Keras callbacks."""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import csv
import multiprocessing
import os
import re
import shutil

import numpy as np

from tensorflow.contrib.keras.python import keras
from tensorflow.contrib.keras.python.keras import testing_utils
from tensorflow.python.platform import test

try:
  import h5py  # pylint:disable=g-import-not-at-top
except ImportError:
  h5py = None


TRAIN_SAMPLES = 10
TEST_SAMPLES = 10
NUM_CLASSES = 2
INPUT_DIM = 3
NUM_HIDDEN = 5
BATCH_SIZE = 5


class KerasCallbacksTest(test.TestCase):

  def test_ModelCheckpoint(self):
    if h5py is None:
      return  # Skip test if models cannot be saved.

    with self.test_session():
      np.random.seed(1337)

      temp_dir = self.get_temp_dir()
      self.addCleanup(shutil.rmtree, temp_dir)

      filepath = os.path.join(temp_dir, 'checkpoint.h5')
      (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
          train_samples=TRAIN_SAMPLES,
          test_samples=TEST_SAMPLES,
          input_shape=(INPUT_DIM,),
          num_classes=NUM_CLASSES)
      y_test = keras.utils.to_categorical(y_test)
      y_train = keras.utils.to_categorical(y_train)
      # case 1
      monitor = 'val_loss'
      save_best_only = False
      mode = 'auto'

      model = keras.models.Sequential()
      model.add(
          keras.layers.Dense(
              NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
      model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))
      model.compile(
          loss='categorical_crossentropy',
          optimizer='rmsprop',
          metrics=['accuracy'])

      cbks = [
          keras.callbacks.ModelCheckpoint(
              filepath,
              monitor=monitor,
              save_best_only=save_best_only,
              mode=mode)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=1,
          verbose=0)
      assert os.path.exists(filepath)
      os.remove(filepath)

      # case 2
      mode = 'min'
      cbks = [
          keras.callbacks.ModelCheckpoint(
              filepath,
              monitor=monitor,
              save_best_only=save_best_only,
              mode=mode)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=1,
          verbose=0)
      assert os.path.exists(filepath)
      os.remove(filepath)

      # case 3
      mode = 'max'
      monitor = 'val_acc'
      cbks = [
          keras.callbacks.ModelCheckpoint(
              filepath,
              monitor=monitor,
              save_best_only=save_best_only,
              mode=mode)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=1,
          verbose=0)
      assert os.path.exists(filepath)
      os.remove(filepath)

      # case 4
      save_best_only = True
      cbks = [
          keras.callbacks.ModelCheckpoint(
              filepath,
              monitor=monitor,
              save_best_only=save_best_only,
              mode=mode)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=1,
          verbose=0)
      assert os.path.exists(filepath)
      os.remove(filepath)

      # case 5
      save_best_only = False
      period = 2
      mode = 'auto'

      filepath = os.path.join(temp_dir, 'checkpoint.{epoch:02d}.h5')
      cbks = [
          keras.callbacks.ModelCheckpoint(
              filepath,
              monitor=monitor,
              save_best_only=save_best_only,
              mode=mode,
              period=period)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=4,
          verbose=0)
      assert os.path.exists(filepath.format(epoch=1))
      assert os.path.exists(filepath.format(epoch=3))
      os.remove(filepath.format(epoch=1))
      os.remove(filepath.format(epoch=3))
      assert not os.path.exists(filepath.format(epoch=0))
      assert not os.path.exists(filepath.format(epoch=2))

  def test_EarlyStopping(self):
    with self.test_session():
      np.random.seed(1337)
      (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
          train_samples=TRAIN_SAMPLES,
          test_samples=TEST_SAMPLES,
          input_shape=(INPUT_DIM,),
          num_classes=NUM_CLASSES)
      y_test = keras.utils.to_categorical(y_test)
      y_train = keras.utils.to_categorical(y_train)
      model = keras.models.Sequential()
      model.add(
          keras.layers.Dense(
              NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
      model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))
      model.compile(
          loss='categorical_crossentropy',
          optimizer='rmsprop',
          metrics=['accuracy'])
      mode = 'max'
      monitor = 'val_acc'
      patience = 0
      cbks = [
          keras.callbacks.EarlyStopping(
              patience=patience, monitor=monitor, mode=mode)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=20,
          verbose=0)

      mode = 'auto'
      monitor = 'val_acc'
      patience = 2
      cbks = [
          keras.callbacks.EarlyStopping(
              patience=patience, monitor=monitor, mode=mode)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=20,
          verbose=0)

  def test_EarlyStopping_reuse(self):
    with self.test_session():
      np.random.seed(1337)
      patience = 3
      data = np.random.random((100, 1))
      labels = np.where(data > 0.5, 1, 0)
      model = keras.models.Sequential((keras.layers.Dense(
          1, input_dim=1, activation='relu'), keras.layers.Dense(
              1, activation='sigmoid'),))
      model.compile(
          optimizer='sgd', loss='binary_crossentropy', metrics=['accuracy'])
      stopper = keras.callbacks.EarlyStopping(monitor='acc', patience=patience)
      weights = model.get_weights()

      hist = model.fit(data, labels, callbacks=[stopper], verbose=0)
      assert len(hist.epoch) >= patience

      # This should allow training to go for at least `patience` epochs
      model.set_weights(weights)
      hist = model.fit(data, labels, callbacks=[stopper], verbose=0)
    assert len(hist.epoch) >= patience

  def test_LearningRateScheduler(self):
    with self.test_session():
      np.random.seed(1337)
      (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
          train_samples=TRAIN_SAMPLES,
          test_samples=TEST_SAMPLES,
          input_shape=(INPUT_DIM,),
          num_classes=NUM_CLASSES)
      y_test = keras.utils.to_categorical(y_test)
      y_train = keras.utils.to_categorical(y_train)
      model = keras.models.Sequential()
      model.add(
          keras.layers.Dense(
              NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
      model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))
      model.compile(
          loss='categorical_crossentropy',
          optimizer='sgd',
          metrics=['accuracy'])

      cbks = [keras.callbacks.LearningRateScheduler(lambda x: 1. / (1. + x))]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=5,
          verbose=0)
      assert (float(keras.backend.get_value(model.optimizer.lr)) - 0.2
             ) < keras.backend.epsilon()

  def test_ReduceLROnPlateau(self):
    with self.test_session():
      np.random.seed(1337)
      (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
          train_samples=TRAIN_SAMPLES,
          test_samples=TEST_SAMPLES,
          input_shape=(INPUT_DIM,),
          num_classes=NUM_CLASSES)
      y_test = keras.utils.to_categorical(y_test)
      y_train = keras.utils.to_categorical(y_train)

      def make_model():
        np.random.seed(1337)
        model = keras.models.Sequential()
        model.add(
            keras.layers.Dense(
                NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
        model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))

        model.compile(
            loss='categorical_crossentropy',
            optimizer=keras.optimizers.SGD(lr=0.1),
            metrics=['accuracy'])
        return model

      model = make_model()

      # This should reduce the LR after the first epoch (due to high epsilon).
      cbks = [
          keras.callbacks.ReduceLROnPlateau(
              monitor='val_loss',
              factor=0.1,
              epsilon=10,
              patience=1,
              cooldown=5)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=5,
          verbose=0)
      assert np.allclose(
          float(keras.backend.get_value(model.optimizer.lr)),
          0.01,
          atol=keras.backend.epsilon())

      model = make_model()
      cbks = [
          keras.callbacks.ReduceLROnPlateau(
              monitor='val_loss', factor=0.1, epsilon=0, patience=1, cooldown=5)
      ]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=5,
          verbose=0)
      assert np.allclose(
          float(keras.backend.get_value(model.optimizer.lr)),
          0.1,
          atol=keras.backend.epsilon())

  def test_CSVLogger(self):
    with self.test_session():
      np.random.seed(1337)
      temp_dir = self.get_temp_dir()
      self.addCleanup(shutil.rmtree, temp_dir)
      filepath = os.path.join(temp_dir, 'log.tsv')

      sep = '\t'
      (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
          train_samples=TRAIN_SAMPLES,
          test_samples=TEST_SAMPLES,
          input_shape=(INPUT_DIM,),
          num_classes=NUM_CLASSES)
      y_test = keras.utils.to_categorical(y_test)
      y_train = keras.utils.to_categorical(y_train)

      def make_model():
        np.random.seed(1337)
        model = keras.models.Sequential()
        model.add(
            keras.layers.Dense(
                NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
        model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))

        model.compile(
            loss='categorical_crossentropy',
            optimizer=keras.optimizers.SGD(lr=0.1),
            metrics=['accuracy'])
        return model

      # case 1, create new file with defined separator
      model = make_model()
      cbks = [keras.callbacks.CSVLogger(filepath, separator=sep)]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=1,
          verbose=0)

      assert os.path.exists(filepath)
      with open(filepath) as csvfile:
        dialect = csv.Sniffer().sniff(csvfile.read())
      assert dialect.delimiter == sep
      del model
      del cbks

      # case 2, append data to existing file, skip header
      model = make_model()
      cbks = [keras.callbacks.CSVLogger(filepath, separator=sep, append=True)]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=1,
          verbose=0)

      # case 3, reuse of CSVLogger object
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=1,
          verbose=0)

      with open(filepath) as csvfile:
        output = ' '.join(csvfile.readlines())
        assert len(re.findall('epoch', output)) == 1

      os.remove(filepath)

  def test_TensorBoard(self):
    np.random.seed(1337)

    temp_dir = self.get_temp_dir()
    self.addCleanup(shutil.rmtree, temp_dir)

    (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
        train_samples=TRAIN_SAMPLES,
        test_samples=TEST_SAMPLES,
        input_shape=(INPUT_DIM,),
        num_classes=NUM_CLASSES)
    y_test = keras.utils.to_categorical(y_test)
    y_train = keras.utils.to_categorical(y_train)

    def data_generator(train):
      if train:
        max_batch_index = len(x_train) // BATCH_SIZE
      else:
        max_batch_index = len(x_test) // BATCH_SIZE
      i = 0
      while 1:
        if train:
          yield (x_train[i * BATCH_SIZE:(i + 1) * BATCH_SIZE],
                 y_train[i * BATCH_SIZE:(i + 1) * BATCH_SIZE])
        else:
          yield (x_test[i * BATCH_SIZE:(i + 1) * BATCH_SIZE],
                 y_test[i * BATCH_SIZE:(i + 1) * BATCH_SIZE])
        i += 1
        i %= max_batch_index

    # case: Sequential
    with self.test_session():
      model = keras.models.Sequential()
      model.add(
          keras.layers.Dense(
              NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
      model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))
      model.compile(
          loss='categorical_crossentropy',
          optimizer='sgd',
          metrics=['accuracy'])

      tsb = keras.callbacks.TensorBoard(
          log_dir=temp_dir, histogram_freq=1, write_images=True)
      cbks = [tsb]

      # fit with validation data
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=3,
          verbose=0)

      # fit with validation data and accuracy
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=2,
          verbose=0)

      # fit generator with validation data
      model.fit_generator(
          data_generator(True),
          len(x_train),
          epochs=2,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          verbose=0)

      # fit generator without validation data
      model.fit_generator(
          data_generator(True),
          len(x_train),
          epochs=2,
          callbacks=cbks,
          verbose=0)

      # fit generator with validation data and accuracy
      model.fit_generator(
          data_generator(True),
          len(x_train),
          epochs=2,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          verbose=0)

      # fit generator without validation data and accuracy
      model.fit_generator(
          data_generator(True), len(x_train), epochs=2, callbacks=cbks)
      assert os.path.exists(temp_dir)

  def test_LambdaCallback(self):
    with self.test_session():
      np.random.seed(1337)
      (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
          train_samples=TRAIN_SAMPLES,
          test_samples=TEST_SAMPLES,
          input_shape=(INPUT_DIM,),
          num_classes=NUM_CLASSES)
      y_test = keras.utils.to_categorical(y_test)
      y_train = keras.utils.to_categorical(y_train)
      model = keras.models.Sequential()
      model.add(
          keras.layers.Dense(
              NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
      model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))
      model.compile(
          loss='categorical_crossentropy',
          optimizer='sgd',
          metrics=['accuracy'])

      # Start an arbitrary process that should run during model
      # training and be terminated after training has completed.
      def target():
        while True:
          pass

      p = multiprocessing.Process(target=target)
      p.start()
      cleanup_callback = keras.callbacks.LambdaCallback(
          on_train_end=lambda logs: p.terminate())

      cbks = [cleanup_callback]
      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=5,
          verbose=0)
      p.join()
      assert not p.is_alive()

  def test_TensorBoard_with_ReduceLROnPlateau(self):
    with self.test_session():
      temp_dir = self.get_temp_dir()
      self.addCleanup(shutil.rmtree, temp_dir)

      (x_train, y_train), (x_test, y_test) = testing_utils.get_test_data(
          train_samples=TRAIN_SAMPLES,
          test_samples=TEST_SAMPLES,
          input_shape=(INPUT_DIM,),
          num_classes=NUM_CLASSES)
      y_test = keras.utils.to_categorical(y_test)
      y_train = keras.utils.to_categorical(y_train)

      model = keras.models.Sequential()
      model.add(
          keras.layers.Dense(
              NUM_HIDDEN, input_dim=INPUT_DIM, activation='relu'))
      model.add(keras.layers.Dense(NUM_CLASSES, activation='softmax'))
      model.compile(
          loss='binary_crossentropy', optimizer='sgd', metrics=['accuracy'])

      cbks = [
          keras.callbacks.ReduceLROnPlateau(
              monitor='val_loss', factor=0.5, patience=4, verbose=1),
          keras.callbacks.TensorBoard(log_dir=temp_dir)
      ]

      model.fit(
          x_train,
          y_train,
          batch_size=BATCH_SIZE,
          validation_data=(x_test, y_test),
          callbacks=cbks,
          epochs=2,
          verbose=0)

      assert os.path.exists(temp_dir)


if __name__ == '__main__':
  test.main()
