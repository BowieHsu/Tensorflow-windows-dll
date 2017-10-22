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

import gzip
import os
import zlib

from tensorflow.contrib.data.python.ops import dataset_ops
from tensorflow.core.example import example_pb2
from tensorflow.core.example import feature_pb2
from tensorflow.python.framework import constant_op
from tensorflow.python.framework import dtypes
from tensorflow.python.framework import errors
from tensorflow.python.framework import ops
from tensorflow.python.lib.io import python_io
from tensorflow.python.ops import array_ops
from tensorflow.python.ops import parsing_ops
from tensorflow.python.platform import test
from tensorflow.python.util import compat


class TextLineDatasetTest(test.TestCase):

  def _lineText(self, f, l):
    return compat.as_bytes("%d: %d" % (f, l))

  def _createFiles(self, num_files, num_lines, crlf=False):
    filenames = []
    for i in range(num_files):
      fn = os.path.join(self.get_temp_dir(), "text_line.%d.txt" % i)
      filenames.append(fn)
      with open(fn, "wb") as f:
        for j in range(num_lines):
          f.write(self._lineText(i, j))
          # Always include a newline after the record unless it is
          # at the end of the file, in which case we include it sometimes.
          if j + 1 != num_lines or i == 0:
            f.write(b"\r\n" if crlf else b"\n")
    return filenames

  def testTextLineDataset(self):
    test_filenames = self._createFiles(2, 5, crlf=True)
    filenames = array_ops.placeholder(dtypes.string, shape=[None])
    num_epochs = array_ops.placeholder(dtypes.int64, shape=[])
    batch_size = array_ops.placeholder(dtypes.int64, shape=[])

    repeat_dataset = dataset_ops.TextLineDataset(filenames).repeat(num_epochs)
    batch_dataset = repeat_dataset.batch(batch_size)

    iterator = dataset_ops.Iterator.from_structure(batch_dataset.output_types)
    init_op = iterator.make_initializer(repeat_dataset)
    init_batch_op = iterator.make_initializer(batch_dataset)
    get_next = iterator.get_next()

    with self.test_session() as sess:
      # Basic test: read from file 0.
      sess.run(init_op, feed_dict={filenames: [test_filenames[0]],
                                   num_epochs: 1})
      for i in range(5):
        self.assertEqual(self._lineText(0, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Basic test: read from file 1.
      sess.run(init_op, feed_dict={filenames: [test_filenames[1]],
                                   num_epochs: 1})
      for i in range(5):
        self.assertEqual(self._lineText(1, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Basic test: read from both files.
      sess.run(init_op, feed_dict={filenames: test_filenames,
                                   num_epochs: 1})
      for j in range(2):
        for i in range(5):
          self.assertEqual(self._lineText(j, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test repeated iteration through both files.
      sess.run(init_op, feed_dict={filenames: test_filenames,
                                   num_epochs: 10})
      for _ in range(10):
        for j in range(2):
          for i in range(5):
            self.assertEqual(self._lineText(j, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test batched and repeated iteration through both files.
      sess.run(init_batch_op, feed_dict={filenames: test_filenames,
                                         num_epochs: 10,
                                         batch_size: 5})
      for _ in range(10):
        self.assertAllEqual([self._lineText(0, i) for i in range(5)],
                            sess.run(get_next))
        self.assertAllEqual([self._lineText(1, i) for i in range(5)],
                            sess.run(get_next))


class FixedLengthRecordReaderTest(test.TestCase):

  def setUp(self):
    super(FixedLengthRecordReaderTest, self).setUp()
    self._num_files = 2
    self._num_records = 7
    self._header_bytes = 5
    self._record_bytes = 3
    self._footer_bytes = 2

  def _record(self, f, r):
    return compat.as_bytes(str(f * 2 + r) * self._record_bytes)

  def _createFiles(self):
    filenames = []
    for i in range(self._num_files):
      fn = os.path.join(self.get_temp_dir(), "fixed_length_record.%d.txt" % i)
      filenames.append(fn)
      with open(fn, "wb") as f:
        f.write(b"H" * self._header_bytes)
        for j in range(self._num_records):
          f.write(self._record(i, j))
        f.write(b"F" * self._footer_bytes)
    return filenames

  def testFixedLengthRecordDataset(self):
    test_filenames = self._createFiles()
    filenames = array_ops.placeholder(dtypes.string, shape=[None])
    num_epochs = array_ops.placeholder(dtypes.int64, shape=[])
    batch_size = array_ops.placeholder(dtypes.int64, shape=[])

    repeat_dataset = (dataset_ops.FixedLengthRecordDataset(
        filenames, self._record_bytes, self._header_bytes, self._footer_bytes)
                      .repeat(num_epochs))
    batch_dataset = repeat_dataset.batch(batch_size)

    iterator = dataset_ops.Iterator.from_structure(batch_dataset.output_types)
    init_op = iterator.make_initializer(repeat_dataset)
    init_batch_op = iterator.make_initializer(batch_dataset)
    get_next = iterator.get_next()

    with self.test_session() as sess:
      # Basic test: read from file 0.
      sess.run(init_op, feed_dict={filenames: [test_filenames[0]],
                                   num_epochs: 1})
      for i in range(self._num_records):
        self.assertEqual(self._record(0, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Basic test: read from file 1.
      sess.run(init_op, feed_dict={filenames: [test_filenames[1]],
                                   num_epochs: 1})
      for i in range(self._num_records):
        self.assertEqual(self._record(1, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Basic test: read from both files.
      sess.run(init_op, feed_dict={filenames: test_filenames,
                                   num_epochs: 1})
      for j in range(self._num_files):
        for i in range(self._num_records):
          self.assertEqual(self._record(j, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test repeated iteration through both files.
      sess.run(init_op, feed_dict={filenames: test_filenames,
                                   num_epochs: 10})
      for _ in range(10):
        for j in range(self._num_files):
          for i in range(self._num_records):
            self.assertEqual(self._record(j, i), sess.run(get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(get_next)

      # Test batched and repeated iteration through both files.
      sess.run(init_batch_op, feed_dict={filenames: test_filenames,
                                         num_epochs: 10,
                                         batch_size: self._num_records})
      for _ in range(10):
        for j in range(self._num_files):
          self.assertAllEqual([self._record(j, i)
                               for i in range(self._num_records)],
                              sess.run(get_next))


class TFRecordDatasetTest(test.TestCase):

  def setUp(self):
    super(TFRecordDatasetTest, self).setUp()
    self._num_files = 2
    self._num_records = 7

    self.test_filenames = self._createFiles()

    self.filenames = array_ops.placeholder(dtypes.string, shape=[None])
    self.num_epochs = array_ops.placeholder_with_default(
        constant_op.constant(1, dtypes.int64), shape=[])
    self.compression_type = array_ops.placeholder_with_default("", shape=[])
    self.batch_size = array_ops.placeholder(dtypes.int64, shape=[])

    repeat_dataset = dataset_ops.TFRecordDataset(
        self.filenames, self.compression_type).repeat(self.num_epochs)
    batch_dataset = repeat_dataset.batch(self.batch_size)

    iterator = dataset_ops.Iterator.from_structure(batch_dataset.output_types)
    self.init_op = iterator.make_initializer(repeat_dataset)
    self.init_batch_op = iterator.make_initializer(batch_dataset)
    self.get_next = iterator.get_next()

  def _record(self, f, r):
    return compat.as_bytes("Record %d of file %d" % (r, f))

  def _createFiles(self):
    filenames = []
    for i in range(self._num_files):
      fn = os.path.join(self.get_temp_dir(), "tf_record.%d.txt" % i)
      filenames.append(fn)
      writer = python_io.TFRecordWriter(fn)
      for j in range(self._num_records):
        writer.write(self._record(i, j))
      writer.close()
    return filenames

  def testReadOneEpoch(self):
    with self.test_session() as sess:
      # Basic test: read from file 0.
      sess.run(self.init_op,
               feed_dict={self.filenames: [self.test_filenames[0]],
                          self.num_epochs: 1})
      for i in range(self._num_records):
        self.assertAllEqual(self._record(0, i), sess.run(self.get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(self.get_next)

      # Basic test: read from file 1.
      sess.run(self.init_op,
               feed_dict={self.filenames: [self.test_filenames[1]],
                          self.num_epochs: 1})
      for i in range(self._num_records):
        self.assertAllEqual(self._record(1, i), sess.run(self.get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(self.get_next)

      # Basic test: read from both files.
      sess.run(self.init_op,
               feed_dict={self.filenames: self.test_filenames,
                          self.num_epochs: 1})
      for j in range(self._num_files):
        for i in range(self._num_records):
          self.assertAllEqual(self._record(j, i), sess.run(self.get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(self.get_next)

  def testReadTenEpochs(self):
    with self.test_session() as sess:
      sess.run(self.init_op, feed_dict={self.filenames: self.test_filenames,
                                        self.num_epochs: 10})
      for _ in range(10):
        for j in range(self._num_files):
          for i in range(self._num_records):
            self.assertAllEqual(self._record(j, i), sess.run(self.get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(self.get_next)

  def testReadTenEpochsOfBatches(self):
    with self.test_session() as sess:
      sess.run(self.init_batch_op,
               feed_dict={self.filenames: self.test_filenames,
                          self.num_epochs: 10,
                          self.batch_size: self._num_records})
      for _ in range(10):
        for j in range(self._num_files):
          values = sess.run(self.get_next)
          self.assertAllEqual([self._record(j, i)
                               for i in range(self._num_records)], values)
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(self.get_next)

  def testReadZlibFiles(self):
    zlib_files = []
    for i, fn in enumerate(self.test_filenames):
      with open(fn, "rb") as f:
        cdata = zlib.compress(f.read())

        zfn = os.path.join(self.get_temp_dir(), "tfrecord_%s.z" % i)
        with open(zfn, "wb") as f:
          f.write(cdata)
        zlib_files.append(zfn)

    with self.test_session() as sess:
      sess.run(self.init_op,
               feed_dict={self.filenames: zlib_files,
                          self.compression_type: "ZLIB"})
      for j in range(self._num_files):
        for i in range(self._num_records):
          self.assertAllEqual(self._record(j, i), sess.run(self.get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(self.get_next)

  def testReadGzipFiles(self):
    gzip_files = []
    for i, fn in enumerate(self.test_filenames):
      with open(fn, "rb") as f:
        gzfn = os.path.join(self.get_temp_dir(), "tfrecord_%s.gz" % i)
        with gzip.GzipFile(gzfn, "wb") as gzf:
          gzf.write(f.read())
        gzip_files.append(gzfn)

    with self.test_session() as sess:
      sess.run(self.init_op,
               feed_dict={self.filenames: gzip_files,
                          self.compression_type: "GZIP"})
      for j in range(self._num_files):
        for i in range(self._num_records):
          self.assertAllEqual(self._record(j, i), sess.run(self.get_next))
      with self.assertRaises(errors.OutOfRangeError):
        sess.run(self.get_next)


class ReadBatchFeaturesTest(test.TestCase):

  def setUp(self):
    super(ReadBatchFeaturesTest, self).setUp()
    self._num_files = 2
    self._num_records = 7
    self.test_filenames = self._createFiles()

  def _read_batch_features(self, filenames, num_epochs, batch_size):
    self.filenames = filenames
    self.num_epochs = num_epochs
    self.batch_size = batch_size

    return dataset_ops.read_batch_features(
        file_pattern=self.filenames,
        batch_size=self.batch_size,
        features={
            "file": parsing_ops.FixedLenFeature([], dtypes.int64),
            "record": parsing_ops.FixedLenFeature([], dtypes.int64),
            "keywords": parsing_ops.VarLenFeature(dtypes.string)
        },
        reader=dataset_ops.TFRecordDataset,
        randomize_input=False,
        num_epochs=self.num_epochs)

  def _record(self, f, r):
    example = example_pb2.Example(features=feature_pb2.Features(
        feature={
            "file":
                feature_pb2.Feature(int64_list=feature_pb2.Int64List(
                    value=[f])),
            "record":
                feature_pb2.Feature(int64_list=feature_pb2.Int64List(
                    value=[r])),
            "keywords":
                feature_pb2.Feature(bytes_list=feature_pb2.BytesList(
                    value=self._get_keywords(f, r)))
        }))
    return example.SerializeToString()

  def _get_keywords(self, f, r):
    num_keywords = 1 + (f + r) % 2
    keywords = []
    for index in range(num_keywords):
      keywords.append(compat.as_bytes("keyword%d" % index))
    return keywords

  def _createFiles(self):
    filenames = []
    for i in range(self._num_files):
      fn = os.path.join(self.get_temp_dir(), "tf_record.%d.txt" % i)
      filenames.append(fn)
      writer = python_io.TFRecordWriter(fn)
      for j in range(self._num_records):
        writer.write(self._record(i, j))
      writer.close()
    return filenames

  def _next_actual_batch(self, sess):
    file_op = self.outputs["file"]
    keywords_indices_op = self.outputs["keywords"].indices
    keywords_values_op = self.outputs["keywords"].values
    keywords_dense_shape_op = self.outputs["keywords"].dense_shape
    record_op = self.outputs["record"]
    return sess.run([
        file_op, keywords_indices_op, keywords_values_op,
        keywords_dense_shape_op, record_op
    ])

  def _next_expected_batch(self, file_indices, batch_size, num_epochs):

    def _next_record(file_indices):
      for j in file_indices:
        for i in range(self._num_records):
          yield j, i

    file_batch = []
    keywords_batch_indices = []
    keywords_batch_values = []
    keywords_batch_max_len = 0
    record_batch = []
    batch_index = 0
    for _ in range(num_epochs):
      for record in _next_record(file_indices):
        f = record[0]
        r = record[1]
        file_batch.append(f)
        record_batch.append(r)
        keywords = self._get_keywords(f, r)
        keywords_batch_values.extend(keywords)
        keywords_batch_indices.extend([[batch_index, i]
                                       for i in range(len(keywords))])
        batch_index += 1
        keywords_batch_max_len = max(keywords_batch_max_len, len(keywords))
        if len(file_batch) == batch_size:
          yield [
              file_batch, keywords_batch_indices, keywords_batch_values,
              [batch_size, keywords_batch_max_len], record_batch
          ]
          file_batch = []
          keywords_batch_indices = []
          keywords_batch_values = []
          keywords_batch_max_len = 0
          record_batch = []
          batch_index = 0
    if file_batch:
      yield [
          file_batch, keywords_batch_indices, keywords_batch_values,
          [len(file_batch), keywords_batch_max_len], record_batch
      ]

  def _verify_records(self, sess, batch_size, file_index=None, num_epochs=1):
    if file_index is not None:
      file_indices = [file_index]
    else:
      file_indices = range(self._num_files)

    for expected_batch in self._next_expected_batch(file_indices, batch_size,
                                                    num_epochs):
      actual_batch = self._next_actual_batch(sess)
      for i in range(len(expected_batch)):
        self.assertAllEqual(expected_batch[i], actual_batch[i])

  def testRead(self):
    for batch_size in [1, 2]:
      for num_epochs in [1, 10]:
        with ops.Graph().as_default():
          with self.test_session(graph=ops.get_default_graph()) as sess:
            # Basic test: read from file 0.
            self.outputs = self._read_batch_features(
                filenames=self.test_filenames[0],
                num_epochs=num_epochs,
                batch_size=batch_size)
            self._verify_records(sess, batch_size, 0, num_epochs=num_epochs)
            with self.assertRaises(errors.OutOfRangeError):
              self._next_actual_batch(sess)

        with ops.Graph().as_default():
          with self.test_session(graph=ops.get_default_graph()) as sess:
            # Basic test: read from file 1.
            self.outputs = self._read_batch_features(
                filenames=self.test_filenames[1],
                num_epochs=num_epochs,
                batch_size=batch_size)
            self._verify_records(sess, batch_size, 1, num_epochs=num_epochs)
            with self.assertRaises(errors.OutOfRangeError):
              self._next_actual_batch(sess)

        with ops.Graph().as_default():
          with self.test_session(graph=ops.get_default_graph()) as sess:
            # Basic test: read from both files.
            self.outputs = self._read_batch_features(
                filenames=self.test_filenames,
                num_epochs=num_epochs,
                batch_size=batch_size)
            self._verify_records(sess, batch_size, num_epochs=num_epochs)
            with self.assertRaises(errors.OutOfRangeError):
              self._next_actual_batch(sess)

if __name__ == "__main__":
  test.main()
