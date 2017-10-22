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
"""Embedding layer.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.contrib.keras.python.keras import backend as K
from tensorflow.contrib.keras.python.keras import constraints
from tensorflow.contrib.keras.python.keras import initializers
from tensorflow.contrib.keras.python.keras import regularizers
from tensorflow.contrib.keras.python.keras.engine import Layer
from tensorflow.python.framework import tensor_shape


class Embedding(Layer):
  """Turns positive integers (indexes) into dense vectors of fixed size.

  eg. [[4], [20]] -> [[0.25, 0.1], [0.6, -0.2]]

  This layer can only be used as the first layer in a model.

  Example:

  ```python
    model = Sequential()
    model.add(Embedding(1000, 64, input_length=10))
    # the model will take as input an integer matrix of size (batch,
    input_length).
    # the largest integer (i.e. word index) in the input should be no larger
    than 999 (vocabulary size).
    # now model.output_shape == (None, 10, 64), where None is the batch
    dimension.

    input_array = np.random.randint(1000, size=(32, 10))

    model.compile('rmsprop', 'mse')
    output_array = model.predict(input_array)
    assert output_array.shape == (32, 10, 64)
  ```

  Arguments:
    input_dim: int > 0. Size of the vocabulary,
        i.e. maximum integer index + 1.
    output_dim: int >= 0. Dimension of the dense embedding.
    embeddings_initializer: Initializer for the `embeddings` matrix.
    embeddings_regularizer: Regularizer function applied to
          the `embeddings` matrix.
    embeddings_constraint: Constraint function applied to
          the `embeddings` matrix.
    mask_zero: Whether or not the input value 0 is a special "padding"
        value that should be masked out.
        This is useful when using recurrent layers,
        which may take variable length inputs.
        If this is `True` then all subsequent layers
        in the model need to support masking or an exception will be raised.
        If mask_zero is set to True, as a consequence, index 0 cannot be
        used in the vocabulary (input_dim should equal size of
        vocabulary + 1).
    input_length: Length of input sequences, when it is constant.
        This argument is required if you are going to connect
        `Flatten` then `Dense` layers upstream
        (without it, the shape of the dense outputs cannot be computed).

  Input shape:
      2D tensor with shape: `(batch_size, sequence_length)`.

  Output shape:
      3D tensor with shape: `(batch_size, sequence_length, output_dim)`.

  References:
      - [A Theoretically Grounded Application of Dropout in Recurrent Neural
        Networks](http://arxiv.org/abs/1512.05287)
  """

  def __init__(self,
               input_dim,
               output_dim,
               embeddings_initializer='uniform',
               embeddings_regularizer=None,
               activity_regularizer=None,
               embeddings_constraint=None,
               mask_zero=False,
               input_length=None,
               **kwargs):
    kwargs['dtype'] = 'int32'
    if 'input_shape' not in kwargs:
      if input_length:
        kwargs['input_shape'] = (input_length,)
      else:
        kwargs['input_shape'] = (None,)
    super(Embedding, self).__init__(**kwargs)

    self.input_dim = input_dim
    self.output_dim = output_dim
    self.embeddings_initializer = initializers.get(embeddings_initializer)
    self.embeddings_regularizer = regularizers.get(embeddings_regularizer)
    self.activity_regularizer = regularizers.get(activity_regularizer)
    self.embeddings_constraint = constraints.get(embeddings_constraint)
    self.mask_zero = mask_zero
    self.input_length = input_length

  def build(self, input_shape):
    input_shape = tensor_shape.TensorShape(input_shape).as_list()
    self.embeddings = self.add_weight(
        shape=(self.input_dim, self.output_dim),
        initializer=self.embeddings_initializer,
        name='embeddings',
        regularizer=self.embeddings_regularizer,
        constraint=self.embeddings_constraint)
    self.built = True

  def compute_mask(self, inputs, mask=None):
    if not self.mask_zero:
      return None
    else:
      return K.not_equal(inputs, 0)

  def _compute_output_shape(self, input_shape):
    input_shape = tensor_shape.TensorShape(input_shape).as_list()
    if not self.input_length:
      input_length = input_shape[1]
    else:
      input_length = self.input_length
    return tensor_shape.TensorShape(
        [input_shape[0], input_length, self.output_dim])

  def call(self, inputs):
    if K.dtype(inputs) != 'int32':
      inputs = K.cast(inputs, 'int32')
    out = K.gather(self.embeddings, inputs)
    return out

  def get_config(self):
    config = {
        'input_dim':
            self.input_dim,
        'output_dim':
            self.output_dim,
        'embeddings_initializer':
            initializers.serialize(self.embeddings_initializer),
        'embeddings_regularizer':
            regularizers.serialize(self.embeddings_regularizer),
        'activity_regularizer':
            regularizers.serialize(self.activity_regularizer),
        'embeddings_constraint':
            constraints.serialize(self.embeddings_constraint),
        'mask_zero':
            self.mask_zero,
        'input_length':
            self.input_length
    }
    base_config = super(Embedding, self).get_config()
    return dict(list(base_config.items()) + list(config.items()))
