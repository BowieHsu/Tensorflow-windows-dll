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

#include "tensorflow/core/grappler/optimizers/meta_optimizer.h"
#include "tensorflow/core/framework/versions.pb.h"
#include "tensorflow/core/grappler/optimizers/auto_parallel.h"
#include "tensorflow/core/grappler/optimizers/constant_folding.h"
#include "tensorflow/core/grappler/optimizers/graph_optimizer.h"
#include "tensorflow/core/grappler/optimizers/layout_optimizer.h"
#include "tensorflow/core/grappler/optimizers/memory_optimizer.h"
#include "tensorflow/core/grappler/optimizers/model_pruner.h"
#include "tensorflow/core/grappler/utils/topological_sort.h"
#include "tensorflow/core/lib/core/status.h"

namespace tensorflow {
namespace grappler {

std::unique_ptr<GraphOptimizer> MetaOptimizer::NewOptimizer(
    const string& optimizer) {
  VLOG(1) << "Adding graph optimization pass: " << optimizer;
  std::unique_ptr<GraphOptimizer> graph_optimizer;
  if (optimizer == "pruning") {
    graph_optimizer.reset(new ModelPruner());
  }
  if (optimizer == "constfold") {
    graph_optimizer.reset(new ConstantFolding());
  }
  if (optimizer == "layout") {
    graph_optimizer.reset(new LayoutOptimizer());
  }
  if (optimizer == "memory") {
    graph_optimizer.reset(new MemoryOptimizer());
  }
  if (optimizer == "autoparallel") {
    graph_optimizer.reset(
        new AutoParallel(cfg_.auto_parallel().num_replicas()));
  }
  return graph_optimizer;
}

Status MetaOptimizer::Optimize(Cluster* cluster, const GrapplerItem& item,
                               GraphDef* optimized_graph) {
  std::vector<std::unique_ptr<GraphOptimizer>> optimizers;
  if (cfg_.optimizers().empty()) {
    if (!cfg_.disable_model_pruning()) {
      optimizers.push_back(std::unique_ptr<GraphOptimizer>(new ModelPruner()));
    }
    if (cfg_.constant_folding()) {
      optimizers.push_back(
          std::unique_ptr<GraphOptimizer>(new ConstantFolding()));
    }
    if (cfg_.optimize_tensor_layout()) {
      optimizers.push_back(
          std::unique_ptr<GraphOptimizer>(new LayoutOptimizer()));
    }
    if (cfg_.memory_optimization() > 0) {
      optimizers.push_back(
          std::unique_ptr<GraphOptimizer>(new MemoryOptimizer()));
    }
    if (cfg_.auto_parallel().enable()) {
      optimizers.push_back(std::unique_ptr<GraphOptimizer>(
          new AutoParallel(cfg_.auto_parallel().num_replicas())));
    }
  } else {
    std::set<string> available_optimizers = {"pruning", "constfold", "layout",
                                             "memory", "autoparallel"};
    for (const auto& optimizer : cfg_.optimizers()) {
      if (available_optimizers.find(optimizer) != available_optimizers.end()) {
        optimizers.push_back(NewOptimizer(optimizer));
      }
    }
  }

  if (optimizers.empty()) {
    *optimized_graph = item.graph;
    return Status::OK();
  }

  bool already_optimized = false;
  for (const auto& optimizer : optimizers) {
    if (!already_optimized) {
      TF_RETURN_IF_ERROR(optimizer->Optimize(cluster, item, optimized_graph));
      already_optimized = true;
    } else {
      GrapplerItem optimized_item = item;
      optimized_item.graph = *optimized_graph;
      TF_RETURN_IF_ERROR(
          optimizer->Optimize(cluster, optimized_item, optimized_graph));
    }
  }
  TopologicalSort(optimized_graph);
  // Copy the graph version.
  *optimized_graph->mutable_versions() = item.graph.versions();

  return Status::OK();
}

void MetaOptimizer::Feedback(Cluster* cluster, const GrapplerItem& item,
                             const GraphDef& pruned_graph, double result) {
  // Nothing to do for MetaOptimizer.
}

bool MetaOptimizerEnabled(const RewriterConfig& cfg) {
  return cfg.optimize_tensor_layout() || cfg.constant_folding() ||
         cfg.auto_parallel().enable() || !cfg.optimizers().empty();
}

Status RunMetaOptimizer(const GrapplerItem& item, const RewriterConfig& cfg,
                        Cluster* cluster, GraphDef* optimized_graph) {
  MetaOptimizer optimizer(cfg);
  return optimizer.Optimize(cluster, item, optimized_graph);
}

}  // namespace grappler
}  // namespace tensorflow
