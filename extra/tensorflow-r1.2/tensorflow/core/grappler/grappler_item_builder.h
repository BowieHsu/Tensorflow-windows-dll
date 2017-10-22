/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef TENSORFLOW_GRAPPLER_GRAPPLER_ITEM_BUILDER_H_
#define TENSORFLOW_GRAPPLER_GRAPPLER_ITEM_BUILDER_H_

#include <memory>
#include <string>
#include "tensorflow/core/grappler/grappler_item.h"

namespace tensorflow {

class MetaGraphDef;

namespace grappler {

struct ItemConfig {
  ItemConfig()
      : ignore_user_placement(true),
        ignore_colocation(true),
        placeholder_unknown_output_shape_dim(-1) {}

  // If true, ignore all user specified node placement.
  bool ignore_user_placement;
  // If true, ignore all user specified colocation attributes.
  bool ignore_colocation;
  // Dimension to use if a placeholder node has an _output_shapes attribute with
  // a dimension of -1.
  int placeholder_unknown_output_shape_dim;
};

// Factory method for creating a GrapplerItem from a MetaGraphDef.
// Returns nullptr if the given meta_graph cannot be converted.
std::unique_ptr<GrapplerItem> GrapplerItemFromMetaGraphDef(
    const string& id, const MetaGraphDef& meta_graph, const ItemConfig& cfg);

}  // end namespace grappler
}  // end namespace tensorflow

#endif  // TENSORFLOW_GRAPPLER_GRAPPLER_ITEM_BUILDER_H_
