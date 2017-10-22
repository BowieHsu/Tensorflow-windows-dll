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

#ifndef TENSORFLOW_GRAPPLER_OPTIMIZERS_GRAPH_REWRITER_H_
#define TENSORFLOW_GRAPPLER_OPTIMIZERS_GRAPH_REWRITER_H_

#include <unordered_map>
#include <unordered_set>
#include "tensorflow/core/grappler/grappler_item.h"

namespace tensorflow {
namespace grappler {

// Tools and utilities to simplify common graph rewrites.
class GraphRewriter {
 public:
  GraphRewriter(const GrapplerItem& item);

  // Forward the inputs of original_node as needed to skip over the nodes that
  // are to be deleted. In other words, if I is an input of 'original_node', and
  // I doesn't belong to one of the nodes in 'nodes_to_delete', I will be an
  // input to 'new_node'. On the other hand, if I belong to a node that will be
  // deleted, I will be replaced with the inputs J of the deleted node (unless J
  // belong to nodes that will be deleted, in which case we'll look for
  // preserved inputs further down the graph).
  void ForwardInputs(const NodeDef& original_node,
                     const std::unordered_set<const NodeDef*>& nodes_to_delete,
                     NodeDef* new_node);

  // Returns true if at least one of the edges in the direct fanout of 'node' is
  // a control dependency edge.
  bool DrivesControlDependency(const NodeDef& node) const;

  // Returns true if at least one of the incident edges is a control dependency
  // edge.
  bool IsDrivenByControlDependency(const NodeDef& node) const;

 private:
  std::unordered_map<string, const NodeDef*> nodes_;
  std::unordered_set<const NodeDef*> control_dependency_drivers_;
};

}  // end namespace grappler
}  // end namespace tensorflow

#endif  // TENSORFLOW_GRAPPLER_OPTIMIZERS_GRAPH_REWRITER_H_
