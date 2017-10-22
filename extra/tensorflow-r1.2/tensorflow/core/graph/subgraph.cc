/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

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

#include "tensorflow/core/graph/subgraph.h"

#include <algorithm>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/node_def_util.h"
#include "tensorflow/core/framework/types.h"
#include "tensorflow/core/graph/algorithm.h"
#include "tensorflow/core/graph/graph.h"
#include "tensorflow/core/graph/graph_constructor.h"
#include "tensorflow/core/graph/node_builder.h"
#include "tensorflow/core/graph/tensor_id.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/logging.h"

namespace tensorflow {

// ----------------------------------------------------------------------------
// Subgraph construction-related routines
// ----------------------------------------------------------------------------
// TODO(vrv): Profile the unordered_set and unordered_map use in this file to
// see if we should use an alternative implementation.

namespace {

// Rewrite graph by replacing the output tensors specified in
// "fed_outputs" with special feed nodes for each specified output
// tensor, and removing any nodes that are now disconnected from the
// part of the graph that reaches the sink node.  The set of special
// feed nodes added to the graph are returned in "*feed_nodes".
//
// Return true on success.  On error, return false and sets *error to
// an appropriate error message (and *g is left in an indeterminate
// state).
static Status FeedInputs(Graph* g, const DeviceAttributes& device_info,
                         const gtl::ArraySlice<string>& fed_outputs,
                         bool use_function_convention,
                         subgraph::NameIndex* name_index,
                         DataTypeVector* out_feed_types) {
  out_feed_types->clear();
  out_feed_types->reserve(fed_outputs.size());
  for (size_t i = 0; i < fed_outputs.size(); ++i) {
    const string& t = fed_outputs[i];
    TensorId id(ParseTensorName(t));

    auto iter = name_index->find(id.first);
    if (iter == name_index->end()) {
      return errors::NotFound("FeedInputs: unable to find feed output ", t);
    }
    const Node* n = iter->second;
    DCHECK_EQ(n->name(), id.first);
    if (id.second >= n->num_outputs()) {
      return errors::InvalidArgument(
          "FeedInputs: ", t, " should have output index < ", n->num_outputs());
    }

    Node* recv_node;

    if (!use_function_convention) {
      TF_RETURN_IF_ERROR(
          NodeBuilder(strings::StrCat("_recv_", id.first, "_", id.second),
                      "_Recv")
              .Attr("tensor_type", BaseType(n->output_type(id.second)))
              .Attr("tensor_name", t)
              .Attr("send_device", device_info.name())
              .Attr("recv_device", device_info.name())
              .Attr("send_device_incarnation",
                    static_cast<int64>(device_info.incarnation()))
              .Attr("client_terminated", true)
              .Finalize(g, &recv_node));
    } else {
      // NOTE(mrry): We must include the index as part of the node
      // name, because _Arg is a "stateful" kernel and therefore
      // its name must uniquely identify a kernel instance across all
      // graphs in the same session.
      TF_RETURN_IF_ERROR(NodeBuilder(strings::StrCat("_arg_", id.first, "_",
                                                     id.second, "_", i),
                                     "_Arg")
                             .Attr("T", BaseType(n->output_type(id.second)))
                             .Attr("index", static_cast<int32>(i))
                             .Finalize(g, &recv_node));
    }
    recv_node->set_assigned_device_name(device_info.name());

    // Copy the _output_shapes from the original node to the feed node,
    // if any.
    std::vector<PartialTensorShape> output_shapes;
    if (GetNodeAttr(n->def(), "_output_shapes", &output_shapes).ok()) {
      if (n->num_outputs() != output_shapes.size()) {
        return errors::InvalidArgument(
            "FeedInputs: ", t,
            ": size of _output_shapes attribute does not "
            "match the number of node outputs");
      }
      std::vector<PartialTensorShape> feed_shapes = {output_shapes[id.second]};
      recv_node->AddAttr("_output_shapes", feed_shapes);
    }

    // Update name_index
    (*name_index)[recv_node->name()] = recv_node;
    g->AddControlEdge(g->source_node(), recv_node);

    // Look through edges coming out of "n" for edges whose src_output() index
    // matches "output_index".  If found, replace the edges with a connection
    // from the special feed node.
    std::vector<const Edge*> to_remove;
    for (const Edge* e : n->out_edges()) {
      if (e->src_output() == id.second) {
        to_remove.emplace_back(e);
      } else if (e->src_output() == Graph::kControlSlot &&
                 (n->def().op() == "Placeholder" ||
                  n->def().op() == "PlaceholderV2")) {
        // When feeding a Placeholder node, any outgoing control edges
        // will be replaced with a control edge from the replacement
        // recv_node.
        // TODO(josh11b,mrry): Come up with a more elegant way of addressing
        // the general version of this problem.
        to_remove.emplace_back(e);
      }
    }

    for (const Edge* e : to_remove) {
      if (e->src_output() == id.second) {
        g->AddEdge(recv_node, 0, e->dst(), e->dst_input());
      } else {
        CHECK_EQ(Graph::kControlSlot, e->src_output());
        g->AddControlEdge(recv_node, e->dst());
      }
      g->RemoveEdge(e);
    }
    out_feed_types->push_back(BaseType(n->output_type(id.second)));
  }
  return Status::OK();
}

static bool AddNodeToTargets(const string& node_or_tensor_name,
                             const subgraph::NameIndex& name_index,
                             std::unordered_set<const Node*>* targets) {
  TensorId id = ParseTensorName(node_or_tensor_name);
  auto iter = name_index.find(id.first);
  if (iter == name_index.end()) {
    return false;
  }
  const Node* n = iter->second;
  CHECK_EQ(n->name(), id.first);
  targets->insert(n);
  return true;
}

static Status PruneForTargets(Graph* g, const subgraph::NameIndex& name_index,
                              const std::vector<Node*>& fetch_nodes,
                              const gtl::ArraySlice<string>& target_nodes) {
  string not_found;
  std::unordered_set<const Node*> targets;
  for (Node* n : fetch_nodes) {
    if (!AddNodeToTargets(n->name(), name_index, &targets)) {
      strings::StrAppend(&not_found, n->name(), " ");
    }
  }
  for (const string& s : target_nodes) {
    if (!AddNodeToTargets(s, name_index, &targets)) {
      strings::StrAppend(&not_found, s, " ");
    }
  }
  if (!not_found.empty()) {
    return errors::NotFound("PruneForTargets: Some target nodes not found: ",
                            not_found);
  }
  PruneForReverseReachability(g, targets);

  // Reconnect nodes with no outgoing edges to the sink node
  FixupSourceAndSinkEdges(g);

  return Status::OK();
}

}  // namespace

namespace subgraph {

Status FetchOutputs(Graph* g, const DeviceAttributes& device_info,
                    const gtl::ArraySlice<string>& fetch_outputs,
                    bool use_function_convention, NameIndex* name_index,
                    std::vector<Node*>* out_fetch_nodes,
                    DataTypeVector* out_fetch_types) {
  out_fetch_nodes->clear();
  out_fetch_nodes->reserve(fetch_outputs.size());
  for (size_t i = 0; i < fetch_outputs.size(); ++i) {
    const string& t = fetch_outputs[i];

    // Parse t into node_name and output_index.
    TensorId id(ParseTensorName(t));

    // Find node in graph with that name.
    auto iter = name_index->find(id.first);
    if (iter == name_index->end()) {
      return errors::NotFound("FetchOutputs node ", t, ": not found");
    }
    Node* n = iter->second;
    DCHECK_EQ(n->name(), id.first);
    VLOG(2) << "Found fetch node for " << t;

    // Validate output_index
    if (n->num_outputs() == 0) {
      return errors::InvalidArgument(
          "Tried to fetch data for '", t,
          "', which produces no output.  To run to a node but not fetch any "
          "data, pass '",
          t,
          "' as an argument to the 'target_node_names' argument of the "
          "Session::Run API.");
    } else if (id.second >= n->num_outputs()) {
      return errors::InvalidArgument("FetchOutputs ", t,
                                     ": output index too large, must be < ",
                                     n->num_outputs());
    }

    // Create the fetch Node and connect it up
    Node* send_node;
    if (!use_function_convention) {
      TF_RETURN_IF_ERROR(
          NodeBuilder(strings::StrCat("_send_", id.first, "_", id.second),
                      "_Send")
              .Input(n, id.second)
              .Attr("tensor_name", t)
              .Attr("send_device", device_info.name())
              .Attr("recv_device", device_info.name())
              .Attr("send_device_incarnation",
                    static_cast<int64>(device_info.incarnation()))
              .Attr("client_terminated", true)
              .Finalize(g, &send_node));
    } else {
      // NOTE(mrry): We must include the index as part of the node
      // name, because _Retval is a "stateful" kernel and therefore
      // its name must uniquely identify a kernel instance across all
      // graphs in the same session.
      TF_RETURN_IF_ERROR(NodeBuilder(strings::StrCat("_retval_", id.first, "_",
                                                     id.second, "_", i),
                                     "_Retval")
                             .Input(n, id.second)
                             .Attr("T", BaseType(n->output_type(id.second)))
                             .Attr("index", static_cast<int32>(i))
                             .Finalize(g, &send_node));
    }
    send_node->set_assigned_device_name(device_info.name());

    // Update the index.
    (*name_index)[send_node->name()] = send_node;

    g->AddControlEdge(send_node, g->sink_node());
    out_fetch_nodes->push_back(send_node);
    out_fetch_types->push_back(BaseType(n->output_type(id.second)));
  }

  return Status::OK();
}

Status RewriteGraphForExecution(
    Graph* g, const gtl::ArraySlice<string>& fed_outputs,
    const gtl::ArraySlice<string>& fetch_outputs,
    const gtl::ArraySlice<string>& target_node_names,
    const DeviceAttributes& device_info, bool use_function_convention,
    RewriteGraphMetadata* out_metadata) {
  if (fetch_outputs.empty() && target_node_names.empty()) {
    return errors::InvalidArgument(
        "Must specify at least one target to fetch or execute.");
  }

  std::unordered_set<string> endpoints;
  for (const string& endpoint_name : fed_outputs) {
    auto result = endpoints.insert(endpoint_name);
    if (!result.second) {
      return errors::InvalidArgument("Endpoint \"", endpoint_name,
                                     "\" fed more than once.");
    }
  }

  for (const auto& fetch : fetch_outputs) {
    if (endpoints.count(fetch) > 0) {
      return errors::InvalidArgument(fetch, " is both fed and fetched.");
    }
  }

  // A separate index mapping name to Node*, for use by FeedInputs,
  // FetchOutputs, and PruneForTargets
  NameIndex name_index;
  name_index.reserve(g->num_nodes());
  for (Node* n : g->nodes()) {
    name_index[n->name()] = n;
  }

  // Add the feeds.  This may replace nodes in the graph, including the nodes
  // currently listed in "fetch_nodes".  We pass "name_index" so the index is
  // kept up to date.
  if (!fed_outputs.empty()) {
    TF_RETURN_IF_ERROR(FeedInputs(g, device_info, fed_outputs,
                                  use_function_convention, &name_index,
                                  &out_metadata->feed_types));
  }

  // Add the fetch nodes, also updating "name_index".
  std::vector<Node*> fetch_nodes;
  if (!fetch_outputs.empty()) {
    TF_RETURN_IF_ERROR(FetchOutputs(g, device_info, fetch_outputs,
                                    use_function_convention, &name_index,
                                    &fetch_nodes, &out_metadata->fetch_types));
  }

  // Prune the graph to only compute what is needed for the fetch nodes and the
  // target nodes.
  if (!fetch_nodes.empty() || !target_node_names.empty()) {
    TF_RETURN_IF_ERROR(
        PruneForTargets(g, name_index, fetch_nodes, target_node_names));
  }

  return Status::OK();
}

}  // namespace subgraph

}  // namespace tensorflow
