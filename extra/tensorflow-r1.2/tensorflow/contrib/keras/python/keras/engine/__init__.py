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
"""The Keras Engine: graph topology and training loop functionality.
"""
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.contrib.keras.python.keras.engine.topology import get_source_inputs
from tensorflow.contrib.keras.python.keras.engine.topology import Input
from tensorflow.contrib.keras.python.keras.engine.topology import InputLayer
from tensorflow.contrib.keras.python.keras.engine.topology import InputSpec
from tensorflow.contrib.keras.python.keras.engine.topology import Layer
from tensorflow.contrib.keras.python.keras.engine.training import Model


# Note: topology.Node is an internal class,
# it isn't meant to be used by Keras users.
