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
"""Utilities related to model visualization."""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import os
import sys

try:
  # pydot-ng is a fork of pydot that is better maintained.
  import pydot_ng as pydot  # pylint: disable=g-import-not-at-top
except ImportError:
  # Fall back on pydot if necessary.
  # Silence a `print` statement that occurs in case of import error,
  # by temporarily replacing sys.stdout.
  _stdout = sys.stdout
  sys.stdout = sys.stderr
  try:
    import pydot  # pylint: disable=g-import-not-at-top
  except ImportError:
    pydot = None
  finally:
    # Restore sys.stdout.
    sys.stdout = _stdout


def _check_pydot():
  if not (pydot and pydot.find_graphviz()):
    raise ImportError('Failed to import pydot. You must install pydot'
                      ' and graphviz for `pydotprint` to work.')


def model_to_dot(model, show_shapes=False, show_layer_names=True):
  """Converts a Keras model to dot format.

  Arguments:
      model: A Keras model instance.
      show_shapes: whether to display shape information.
      show_layer_names: whether to display layer names.

  Returns:
      A `pydot.Dot` instance representing the Keras model.
  """
  from tensorflow.contrib.keras.python.keras.layers.wrappers import Wrapper  # pylint: disable=g-import-not-at-top
  from tensorflow.contrib.keras.python.keras.models import Sequential  # pylint: disable=g-import-not-at-top

  _check_pydot()
  dot = pydot.Dot()
  dot.set('rankdir', 'TB')
  dot.set('concentrate', True)
  dot.set_node_defaults(shape='record')

  if isinstance(model, Sequential):
    if not model.built:
      model.build()
    model = model.model
  layers = model.layers

  # Create graph nodes.
  for layer in layers:
    layer_id = str(id(layer))

    # Append a wrapped layer's label to node's label, if it exists.
    layer_name = layer.name
    class_name = layer.__class__.__name__
    if isinstance(layer, Wrapper):
      layer_name = '{}({})'.format(layer_name, layer.layer.name)
      child_class_name = layer.layer.__class__.__name__
      class_name = '{}({})'.format(class_name, child_class_name)

    # Create node's label.
    if show_layer_names:
      label = '{}: {}'.format(layer_name, class_name)
    else:
      label = class_name

    # Rebuild the label as a table including input/output shapes.
    if show_shapes:
      try:
        outputlabels = str(layer.output_shape)
      except AttributeError:
        outputlabels = 'multiple'
      if hasattr(layer, 'input_shape'):
        inputlabels = str(layer.input_shape)
      elif hasattr(layer, 'input_shapes'):
        inputlabels = ', '.join([str(ishape) for ishape in layer.input_shapes])
      else:
        inputlabels = 'multiple'
      label = '%s\n|{input:|output:}|{{%s}|{%s}}' % (label, inputlabels,
                                                     outputlabels)

    node = pydot.Node(layer_id, label=label)
    dot.add_node(node)

  # Connect nodes with edges.
  for layer in layers:
    layer_id = str(id(layer))
    for i, node in enumerate(layer.inbound_nodes):
      node_key = layer.name + '_ib-' + str(i)
      if node_key in model.container_nodes:
        for inbound_layer in node.inbound_layers:
          inbound_layer_id = str(id(inbound_layer))
          layer_id = str(id(layer))
          dot.add_edge(pydot.Edge(inbound_layer_id, layer_id))
  return dot


def plot_model(model,
               to_file='model.png',
               show_shapes=False,
               show_layer_names=True):
  dot = model_to_dot(model, show_shapes, show_layer_names)
  _, extension = os.path.splitext(to_file)
  if not extension:
    extension = 'png'
  else:
    extension = extension[1:]
  dot.write(to_file, format=extension)
