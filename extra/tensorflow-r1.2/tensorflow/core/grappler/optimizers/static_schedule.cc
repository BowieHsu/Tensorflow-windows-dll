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

#include "tensorflow/core/grappler/optimizers/static_schedule.h"
#include <deque>
#include "tensorflow/core/framework/attr_value.pb.h"
#include "tensorflow/core/grappler/costs/graph_properties.h"
#include "tensorflow/core/grappler/costs/op_level_cost_estimator.h"
#include "tensorflow/core/grappler/costs/virtual_placer.h"
#include "tensorflow/core/grappler/op_types.h"
#include "tensorflow/core/grappler/utils.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/strings/strcat.h"

namespace tensorflow {
namespace grappler {

static Costs::NanoSeconds PredictExecutionTime(
    const GraphProperties& properties, const OpLevelCostEstimator& estimator,
    const VirtualPlacer& placer, const NodeDef& node) {
  OpInfo op_features;
  op_features.set_op(node.op());
  *op_features.mutable_attr() = node.attr();

  std::vector<OpInfo::TensorProperties> inputs =
      properties.GetInputProperties(node.name());
  for (auto& input : inputs) {
    op_features.add_inputs()->Swap(&input);
  }

  DeviceProperties device = placer.get_device(node);
  op_features.mutable_device()->Swap(&device);

  Costs::NanoSeconds estimate =
      estimator.PredictCosts(op_features).execution_time;

  // Make sure our estimates are at least one nanosecond per node.
  return std::max(estimate, Costs::NanoSeconds(1));
}

Status EstimateEarliestExecutionTimes(
    const GrapplerItem& item, const Cluster* cluster,
    std::unordered_map<const NodeDef*, Costs::NanoSeconds>* completion_times) {
  std::unordered_map<string, const NodeDef*> name_map;
  std::unordered_map<const NodeDef*, int> pending_inputs;
  std::deque<const NodeDef*> ready_nodes;
  for (const NodeDef& node : item.graph.node()) {
    name_map[node.name()] = &node;
    if (node.input_size() == 0) {
      ready_nodes.push_back(&node);
      (*completion_times)[&node] = 0;
    } else if (IsMerge(node)) {
      // Merge nodes are processed as soon as one of the input becomes
      // available.
      pending_inputs[&node] = 1;
    } else {
      pending_inputs[&node] = node.input_size();
    }
  }

  std::unordered_map<const NodeDef*, std::vector<const NodeDef*>> fanouts;
  for (const NodeDef& node : item.graph.node()) {
    for (const string& input : node.input()) {
      string node_name = NodeName(input);
      auto it = name_map.find(node_name);
      if (it == name_map.end()) {
        return errors::InvalidArgument(
            strings::StrCat("Unknown input node ", input));
      }
      const NodeDef* fanin = it->second;
      fanouts[fanin].push_back(&node);
    }
  }
  name_map.clear();

  GraphProperties properties(item);
  TF_RETURN_IF_ERROR(properties.InferStatically());
  OpLevelCostEstimator estimator;
  VirtualPlacer placer(cluster);

  while (!ready_nodes.empty()) {
    const NodeDef* node = ready_nodes.front();
    ready_nodes.pop_front();

    Costs::NanoSeconds execution_time =
        PredictExecutionTime(properties, estimator, placer, *node);
    Costs::NanoSeconds completion_time =
        execution_time + (*completion_times)[node];
    (*completion_times)[node] = completion_time;

    for (const NodeDef* fanout : fanouts[node]) {
      int pending = pending_inputs[fanout];
      if (pending == 0) {
        // Already processed. Avoid going through loops more than once.
        continue;
      } else if (pending == 1) {
        ready_nodes.push_back(fanout);
      }
      pending_inputs[fanout]--;

      Costs::NanoSeconds ready_time =
          std::max(completion_time, (*completion_times)[fanout]);
      (*completion_times)[fanout] = ready_time;
    }
  }

  return Status::OK();
}

}  // end namespace grappler
}  // end namespace tensorflow
