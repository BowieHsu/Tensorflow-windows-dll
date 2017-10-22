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
"""Utilities related to disk I/O."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from collections import defaultdict
import sys

import numpy as np


try:
  import h5py  # pylint:disable=g-import-not-at-top
except ImportError:
  h5py = None


class HDF5Matrix(object):
  """Representation of HDF5 dataset to be used instead of a Numpy array.

  Example:

  ```python
      x_data = HDF5Matrix('input/file.hdf5', 'data')
      model.predict(x_data)
  ```

  Providing `start` and `end` allows use of a slice of the dataset.

  Optionally, a normalizer function (or lambda) can be given. This will
  be called on every slice of data retrieved.

  Arguments:
      datapath: string, path to a HDF5 file
      dataset: string, name of the HDF5 dataset in the file specified
          in datapath
      start: int, start of desired slice of the specified dataset
      end: int, end of desired slice of the specified dataset
      normalizer: function to be called on data when retrieved

  Returns:
      An array-like HDF5 dataset.
  """
  refs = defaultdict(int)

  def __init__(self, datapath, dataset, start=0, end=None, normalizer=None):
    if h5py is None:
      raise ImportError('The use of HDF5Matrix requires '
                        'HDF5 and h5py installed.')

    if datapath not in list(self.refs.keys()):
      f = h5py.File(datapath)
      self.refs[datapath] = f
    else:
      f = self.refs[datapath]
    self.data = f[dataset]
    self.start = start
    if end is None:
      self.end = self.data.shape[0]
    else:
      self.end = end
    self.normalizer = normalizer

  def __len__(self):
    return self.end - self.start

  def __getitem__(self, key):
    if isinstance(key, slice):
      if key.stop + self.start <= self.end:
        idx = slice(key.start + self.start, key.stop + self.start)
      else:
        raise IndexError
    elif isinstance(key, int):
      if key + self.start < self.end:
        idx = key + self.start
      else:
        raise IndexError
    elif isinstance(key, np.ndarray):
      if np.max(key) + self.start < self.end:
        idx = (self.start + key).tolist()
      else:
        raise IndexError
    elif isinstance(key, list):
      if max(key) + self.start < self.end:
        idx = [x + self.start for x in key]
      else:
        raise IndexError
    else:
      raise IndexError
    if self.normalizer is not None:
      return self.normalizer(self.data[idx])
    else:
      return self.data[idx]

  @property
  def shape(self):
    return (self.end - self.start,) + self.data.shape[1:]


def ask_to_proceed_with_overwrite(filepath):
  """Produces a prompt asking about overwriting a file.

  Arguments:
      filepath: the path to the file to be overwritten.

  Returns:
      True if we can proceed with overwrite, False otherwise.
  """
  get_input = input
  if sys.version_info[:2] <= (2, 7):
    get_input = raw_input
  overwrite = get_input('[WARNING] %s already exists - overwrite? '
                        '[y/n]' % (filepath))
  while overwrite not in ['y', 'n']:
    overwrite = get_input('Enter "y" (overwrite) or "n" (cancel).')
  if overwrite == 'n':
    return False
  print('[TIP] Next time specify overwrite=True!')
  return True
