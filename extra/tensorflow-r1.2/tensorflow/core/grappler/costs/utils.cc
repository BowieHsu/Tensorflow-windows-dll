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

#include "tensorflow/core/grappler/costs/utils.h"

#include <stddef.h>
#include <utility>

#include "third_party/eigen3/Eigen/Core"

#if GOOGLE_CUDA
#include "cuda/include/cuda.h"
#include "cuda/include/cuda_runtime_api.h"
#include "cuda/include/cudnn.h"
#endif

#include "tensorflow/core/framework/attr_value.pb.h"
#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/op_def.pb.h"
#include "tensorflow/core/framework/tensor_shape.pb.h"
#include "tensorflow/core/framework/types.pb.h"
#include "tensorflow/core/graph/graph.h"
#include "tensorflow/core/graph/tensor_id.h"
#include "tensorflow/core/grappler/clusters/utils.h"
#include "tensorflow/core/lib/strings/numbers.h"
#include "tensorflow/core/lib/strings/strcat.h"
#include "tensorflow/core/platform/cpu_info.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/protobuf.h"
#include "tensorflow/core/util/device_name_utils.h"

namespace tensorflow {
namespace grappler {

static OpInfo::TensorProperties UnknownInput() {
  OpInfo::TensorProperties input;
  input.set_dtype(DataType::DT_INVALID);
  input.mutable_shape()->set_unknown_rank(true);
  return input;
}

static std::vector<TensorProto> ExtractTensors(const AttrValue& attr_value) {
  std::vector<TensorProto> tensors;
  switch (attr_value.value_case()) {
    case AttrValue::kTensor: {
      tensors.push_back(attr_value.tensor());
      break;
    }
    case AttrValue::kList: {
      for (const auto& tensor_proto : attr_value.list().tensor()) {
        tensors.push_back(tensor_proto);
      }
      break;
    }
    default: {}
  }
  return tensors;
}

static void ExtractExtraProperties(
    const NodeDef& node,
    const std::unordered_map<string, const NodeDef*>& name_to_node,
    std::vector<OpInfo::TensorProperties>* extra_inputs,
    protobuf::Map<string, AttrValue>* attr_map) {
  OpRegistry* op_registry = OpRegistry::Global();
  const OpDef* op_def;
  auto s = op_registry->LookUpOpDef(node.op(), &op_def);
  if (!s.ok()) {
    op_def = nullptr;
  }

  for (int i = 0; i < node.input_size(); ++i) {
    const string input_name = node.input(i);
    CHECK(!input_name.empty());
    TensorId input_tensor_id = ParseTensorName(input_name);
    const string input_node_name = input_tensor_id.first.ToString();

    auto iter = name_to_node.find(input_node_name);
    if (iter == name_to_node.end()) continue;
    const NodeDef* input_node = iter->second;

    // The value attribute in Const input is useful for cost prediction.
    if (input_node->op() == "Const") {
      auto it = input_node->attr().find("value");
      if (it == input_node->attr().end()) continue;

      const AttrValue& attr_value = it->second;
      std::vector<TensorProto> tensors = ExtractTensors(attr_value);
      if (tensors.empty()) continue;

      const TensorProto& t = tensors[0];
      OpInfo::TensorProperties input;
      input.set_dtype(t.dtype());
      *(input.mutable_shape()) = t.tensor_shape();
      *(input.mutable_value()) = t;
      extra_inputs->push_back(input);

      // For filename input, the file size can also be useful.
      if (op_def &&
          op_def->input_arg(i).name().find("filename") != std::string::npos) {
        Tensor tensor;
        CHECK(tensor.FromProto(t));
        const string filename = tensor.scalar<string>()();

        Env* env = Env::Default();
        FileStatistics stat;
        Status s = env->Stat(filename, &stat);
        if (s.ok()) {
          AttrValue attr;
          attr.set_i(stat.length);
          string attr_key = strings::StrCat("input_", i, "_filesize");
          (*attr_map)[attr_key] = attr;
        }
      }
    }

    // When the input is a handle (e.g. look up table handle), the information
    // in the op itself is not sufficient to predict the op memory.
    if (op_def &&
        op_def->input_arg(i).name().find("handle") != std::string::npos) {
      string new_key = strings::StrCat("parent_", i, "_op");
      AttrValue attr;
      attr.set_s(input_node->op());
      (*attr_map)[new_key] = attr;
      // TODO(yuefengz): Only parent node's op name is copied. Copy inputs
      // and attributes when necessary.
    }
  }
}

std::vector<OpInfo::TensorProperties> FindInputFeatures(
    const NodeDef& node,
    const std::unordered_map<string, const CostGraphDef::Node*>& name_to_cost,
    const std::unordered_map<string, const NodeDef*>& name_to_node) {
  std::vector<OpInfo::TensorProperties> inputs;
  for (const auto& input_name : node.input()) {
    CHECK(!input_name.empty());
    TensorId input_tensor_id = ParseTensorName(input_name);
    const string input_node_name = input_tensor_id.first.ToString();
    const int output_index = input_tensor_id.second;

    // Skip control inputs.
    if (output_index == Graph::kControlSlot) {
      continue;
    }

    auto it = name_to_cost.find(input_node_name);
    if (it == name_to_cost.end() || output_index < 0) {
      inputs.push_back(UnknownInput());
    } else {
      const CostGraphDef::Node* input_cost = it->second;
      const CostGraphDef::Node::OutputInfo& output =
          input_cost->output_info(output_index);
      OpInfo::TensorProperties input;
      input.set_dtype(output.dtype());
      *input.mutable_shape() = output.shape();
      inputs.push_back(input);
    }
  }

  return inputs;
}

DeviceProperties GetDeviceInfo(const string& device_str) {
  DeviceNameUtils::ParsedName parsed;
  if (DeviceNameUtils::ParseFullName(device_str, &parsed)) {
    if (parsed.type == "GPU") {
      return GetLocalGPUInfo(parsed.id);
    } else if (parsed.type == "CPU") {
      return GetLocalCPUInfo();
    }
  }
  DeviceProperties device;
  device.set_type("UNKNOWN");
  return device;
}

DeviceProperties GetDeviceInfo(const CostGraphDef::Node& node) {
  return GetDeviceInfo(node.device());
}

OpInfo BuildOpInfo(
    const NodeDef& node, const string& device_str,
    const std::unordered_map<string, const NodeDef*>& name_to_node,
    const std::vector<OpInfo::TensorProperties>& inputs) {
  OpInfo op_info;
  op_info.set_op(node.op());
  *op_info.mutable_attr() = node.attr();
  *op_info.mutable_device() = GetDeviceInfo(device_str);
  for (auto& input : inputs) {
    *op_info.add_inputs() = input;
  }

  std::vector<OpInfo::TensorProperties> extra_inputs;
  ExtractExtraProperties(node, name_to_node, &extra_inputs,
                         op_info.mutable_attr());
  for (auto& input : extra_inputs) {
    *op_info.add_inputs() = input;
  }

  return op_info;
}

}  // end namespace grappler
}  // end namespace tensorflow
