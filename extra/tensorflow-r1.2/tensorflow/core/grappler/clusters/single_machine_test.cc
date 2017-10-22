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

#include "tensorflow/core/grappler/clusters/single_machine.h"
#include "tensorflow/cc/framework/scope.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/cost_graph.pb.h"
#include "tensorflow/core/framework/step_stats.pb.h"
#include "tensorflow/core/grappler/grappler_item.h"
#include "tensorflow/core/grappler/inputs/trivial_test_graph_input_yielder.h"
#include "tensorflow/core/grappler/utils.h"
#include "tensorflow/core/platform/protobuf.h"
#include "tensorflow/core/platform/test.h"

namespace tensorflow {
namespace grappler {
namespace {

class SingleMachineTest : public ::testing::Test {
 public:
  void SetUp() override {
    // Provision a single machine with 3 cpu cores
    cluster_.reset(new SingleMachine(5 * 60, 3, 0));
    TF_CHECK_OK(cluster_->Provision());
  }

  void TearDown() override {
    cluster_.reset();
  }

 protected:
  std::unique_ptr<SingleMachine> cluster_;
};

TEST_F(SingleMachineTest, CostModel) {
  TrivialTestGraphInputYielder fake_input(4, 1, 10, false,
                                          cluster_->GetDeviceNames());
  GrapplerItem item;
  CHECK(fake_input.NextItem(&item));

  TF_CHECK_OK(cluster_->Initialize(item));

  RunMetadata metadata;
  const int64 start_micros = Env::Default()->NowMicros();
  TF_CHECK_OK(cluster_->Run(item.graph, item.feed, item.fetch, &metadata));
  const int64 run_duration_micros = Env::Default()->NowMicros() - start_micros;

  // There should be at least 4 nodes corresponding to the 4 stages we created
  // in the fake input.
  EXPECT_LE(4, metadata.cost_graph().node_size());
  for (const auto& node : metadata.cost_graph().node()) {
    // Skip the special nodes inserted by TF: these are prefixed with an
    // underscore.
    if (node.name()[0] == '_' || node.name().find("/_") != string::npos) {
      continue;
    }
    EXPECT_EQ(1, node.output_info_size());
    EXPECT_LE(8, node.output_info(0).size());
    const TensorShapeProto& shape = node.output_info(0).shape();
    EXPECT_EQ(2, shape.dim_size());
    EXPECT_EQ(10, shape.dim(0).size());
    EXPECT_EQ(1, shape.dim(1).size());
    EXPECT_LE(0, node.compute_cost());
    EXPECT_GE(run_duration_micros, node.compute_cost());
  }
}

TEST_F(SingleMachineTest, Queue) {
  TrivialTestGraphInputYielder fake_input(4, 1, 10, true,
                                          cluster_->GetDeviceNames());
  GrapplerItem item;
  CHECK(fake_input.NextItem(&item));

  TF_CHECK_OK(cluster_->Initialize(item));
  RunMetadata metadata;
  TF_CHECK_OK(cluster_->Run(item.graph, item.feed, item.fetch, &metadata));
}

TEST_F(SingleMachineTest, MultipleItems) {
  TrivialTestGraphInputYielder fake_input(4, 1, 10, false,
                                          cluster_->GetDeviceNames());

  for (int i = 0; i < 3; ++i) {
    GrapplerItem item;
    CHECK(fake_input.NextItem(&item));
    TF_CHECK_OK(cluster_->Initialize(item));
    RunMetadata metadata1;
    TF_CHECK_OK(cluster_->Run(item.graph, item.feed, item.fetch, &metadata1));
    RunMetadata metadata2;
    TF_CHECK_OK(cluster_->Run(item.graph, item.feed, item.fetch, &metadata2));

    // There should be at least 4 nodes corresponding to the 4 stages we created
    // in the fake input, plus 1 enqueue and 1 dequeue node.
    EXPECT_LE(6, metadata1.cost_graph().node_size());
    for (const auto& node : metadata1.cost_graph().node()) {
      if (node.name()[0] == '_' || node.name().find("/_") != string::npos ||
          node.name() == "queue") {
        continue;
      }
      EXPECT_EQ(1, node.output_info_size());
      const TensorShapeProto& shape = node.output_info(0).shape();
      EXPECT_EQ(2, shape.dim_size());
      EXPECT_EQ(10, shape.dim(0).size());
      EXPECT_EQ(1, shape.dim(1).size());
    }

    for (int i = 0; i < metadata1.cost_graph().node_size(); ++i) {
      metadata1.mutable_cost_graph()->mutable_node(i)->set_compute_cost(0);
      metadata1.clear_step_stats();
    }
    for (int i = 0; i < metadata2.cost_graph().node_size(); ++i) {
      metadata2.mutable_cost_graph()->mutable_node(i)->set_compute_cost(0);
      metadata2.clear_step_stats();
    }
    string s1;
    ::tensorflow::protobuf::TextFormat::PrintToString(metadata1, &s1);
    string s2;
    ::tensorflow::protobuf::TextFormat::PrintToString(metadata2, &s2);
    EXPECT_EQ(s1, s2);
  }
}

TEST_F(SingleMachineTest, InitializationMemory) {
  // Build a variable and its initialization graph.
  tensorflow::Scope s = tensorflow::Scope::NewRootScope();
  int batch_size = 10;
  Output x =
      ops::RandomNormal(s.WithOpName("x"), {batch_size, 1}, DataType::DT_FLOAT);
  Output v = ops::Variable(s.WithOpName("v"), TensorShape({batch_size, 1}),
                           DataType::DT_FLOAT);
  Output init = ops::Assign(s.WithOpName("init"), v, x);

  GrapplerItem item;
  TF_CHECK_OK(s.ToGraphDef(&item.graph));
  item.init_ops.push_back(init.name());
  item.fetch.push_back(v.name());

  TF_CHECK_OK(cluster_->Initialize(item));
  RunMetadata metadata;
  TF_CHECK_OK(cluster_->Run(item.graph, item.feed, item.fetch, &metadata));

  // Check that the initialization op is present in the cost model.
  bool found = false;
  for (const auto& node : metadata.cost_graph().node()) {
    found |= (node.name() == NodeName(init.name()));
  }
  EXPECT_TRUE(found);
}

namespace {
template <class T>
inline void SetNodeAttr(const string& key, const T& value, NodeDef* node) {
  AttrValue attr_value;
  SetAttrValue(value, &attr_value);
  auto* attr_map = node->mutable_attr();
  (*attr_map)[key] = attr_value;
}
template <>
inline void SetNodeAttr(const string& key, const Tensor& tensor,
                        NodeDef* node) {
  TensorProto tensor_proto;
  tensor.AsProtoTensorContent(&tensor_proto);
  SetNodeAttr(key, tensor_proto, node);
}

}  // namespace

TEST_F(SingleMachineTest, PersistentMemory) {
  // Build a hashtable and its initialization graph.
  GrapplerItem item;
  const DataType key_dtype = DT_INT64;
  const DataType data_dtype = DT_INT64;

  NodeDef* hashtable_node = item.graph.add_node();
  hashtable_node->set_op("HashTable");
  hashtable_node->set_name("hash_table");
  SetNodeAttr("key_dtype", key_dtype, hashtable_node);
  SetNodeAttr("value_dtype", data_dtype, hashtable_node);

  // Initial hashtable keys and values
  NodeDef* keys_node = item.graph.add_node();
  keys_node->set_op("Const");
  keys_node->set_name("table_keys");
  SetNodeAttr("dtype", key_dtype, keys_node);
  Tensor keys(key_dtype, TensorShape{2});
  keys.vec<int64>()(0) = 123;
  keys.vec<int64>()(1) = 321;
  SetNodeAttr("value", keys, keys_node);

  NodeDef* values_node = item.graph.add_node();
  values_node->set_op("Const");
  values_node->set_name("table_values");
  SetNodeAttr("dtype", data_dtype, values_node);
  Tensor values(data_dtype, TensorShape{2});
  values.vec<int64>()(0) = 789;
  values.vec<int64>()(1) = 987;
  SetNodeAttr("value", values, values_node);

  // InitializeTable node
  NodeDef* init_table_node = item.graph.add_node();
  init_table_node->set_op("InitializeTable");
  init_table_node->set_name("initialize_table");
  SetNodeAttr("Tkey", key_dtype, init_table_node);
  SetNodeAttr("Tval", data_dtype, init_table_node);
  *init_table_node->add_input() = "hash_table";
  *init_table_node->add_input() = "table_keys";
  *init_table_node->add_input() = "table_values";
  item.init_ops.push_back(init_table_node->name());

  // Key to lookup
  NodeDef* query_node = item.graph.add_node();
  query_node->set_op("Const");
  query_node->set_name("query");
  SetNodeAttr("dtype", key_dtype, query_node);
  Tensor query(key_dtype, TensorShape({}));
  query.flat<int64>()(0) = 0;
  SetNodeAttr("value", query, query_node);

  // Default return value of hashtable lookup
  NodeDef* default_value_node = item.graph.add_node();
  default_value_node->set_op("Const");
  default_value_node->set_name("default_table_value");
  SetNodeAttr("dtype", data_dtype, default_value_node);
  Tensor dflt(data_dtype, TensorShape({}));
  dflt.flat<int64>()(0) = 456;
  SetNodeAttr("value", dflt, default_value_node);

  // HashTable lookup node
  NodeDef* lookup_node = item.graph.add_node();
  lookup_node->set_op("LookupTableFind");
  lookup_node->set_name("table_lookup");
  SetNodeAttr("Tin", key_dtype, lookup_node);
  SetNodeAttr("Tout", data_dtype, lookup_node);
  *lookup_node->add_input() = "hash_table";
  *lookup_node->add_input() = "query";
  *lookup_node->add_input() = "default_table_value";
  item.fetch.push_back(lookup_node->name());

  // Run the graph
  TF_CHECK_OK(cluster_->Initialize(item));
  RunMetadata metadata;
  TF_CHECK_OK(cluster_->Run(item.graph, item.feed, item.fetch, &metadata));

  // Check the cost model.
  bool found_table_init = false;
  bool found_hashtable = false;
  for (const auto& node : metadata.cost_graph().node()) {
    if (node.name() == "hash_table") {
      found_hashtable = true;
      // Persistent memory usage should be 0 since it's recorded as part of the
      // initialize_table op.
      EXPECT_EQ(0, node.host_persistent_memory_size());
      EXPECT_EQ(0, node.device_persistent_memory_size());
    } else if (node.name() == "initialize_table") {
      found_table_init = true;
      // Persistent memory should hold 2 keys and 2 values.
      EXPECT_LE(4 * sizeof(int64), node.host_persistent_memory_size());
      EXPECT_EQ(0, node.device_persistent_memory_size());
    }
  }
  EXPECT_TRUE(found_table_init);
  EXPECT_TRUE(found_hashtable);
}

}  // namespace
}  // namespace grappler
}  // namespace tensorflow
