# Copyright 2015 The TensorFlow Authors. All Rights Reserved.
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
"""Boston housing price regression dataset.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np

from tensorflow.contrib.keras.python.keras.utils.data_utils import get_file


def load_data(path='boston_housing.npz', seed=113, test_split=0.2):
  """Loads the Boston Housing dataset.

  Arguments:
      path: path where to cache the dataset locally
          (relative to ~/.keras/datasets).
      seed: Random seed for shuffling the data
          before computing the test split.
      test_split: fraction of the data to reserve as test set.

  Returns:
      Tuple of Numpy arrays: `(x_train, y_train), (x_test, y_test)`.
  """
  assert 0 <= test_split < 1
  path = get_file(
      path, origin='https://s3.amazonaws.com/keras-datasets/boston_housing.npz')
  f = np.load(path)
  x = f['x']
  y = f['y']
  f.close()

  np.random.seed(seed)
  np.random.shuffle(x)
  np.random.seed(seed)
  np.random.shuffle(y)

  x_train = np.array(x[:int(len(x) * (1 - test_split))])
  y_train = np.array(y[:int(len(x) * (1 - test_split))])
  x_test = np.array(x[int(len(x) * (1 - test_split)):])
  y_test = np.array(y[int(len(x) * (1 - test_split)):])
  return (x_train, y_train), (x_test, y_test)
