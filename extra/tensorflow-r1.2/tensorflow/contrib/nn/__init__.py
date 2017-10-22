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
"""Module for deprecated ops in tf.nn.

@@deprecated_flipped_softmax_cross_entropy_with_logits
@@deprecated_flipped_sparse_softmax_cross_entropy_with_logits
@@deprecated_flipped_sigmoid_cross_entropy_with_logits
"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

# pylint: disable=unused-import,wildcard-import
from tensorflow.contrib.nn.python.ops.cross_entropy import *
# pylint: enable=unused-import,wildcard-import

from tensorflow.python.util.all_util import remove_undocumented
remove_undocumented(__name__)
