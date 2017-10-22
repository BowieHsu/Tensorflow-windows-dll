// Copyright 2017 The TensorFlow Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =============================================================================
#ifndef THIRD_PARTY_TENSORFLOW_CONTRIB_BOOSTED_TREES_LIB_UTILS_BATCH_FEATURES_H_
#define THIRD_PARTY_TENSORFLOW_CONTRIB_BOOSTED_TREES_LIB_UTILS_BATCH_FEATURES_H_

#include <vector>
#include "tensorflow/contrib/boosted_trees/lib/utils/examples_iterable.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_types.h"
#include "tensorflow/core/platform/macros.h"
#include "tensorflow/core/util/sparse/sparse_tensor.h"

namespace tensorflow {
namespace boosted_trees {
namespace utils {

class BatchFeatures {
 public:
  // Constructs batch features with a fixed batch size.
  explicit BatchFeatures(int64 batch_size) : batch_size_(batch_size) {}

  // Disallow copy and assign.
  BatchFeatures(const BatchFeatures& other) = delete;
  BatchFeatures& operator=(const BatchFeatures& other) = delete;

  // Method to initialize batch features from op kernel context.
  Status Initialize(std::vector<Tensor> dense_float_features_list,
                    std::vector<Tensor> sparse_float_feature_indices_list,
                    std::vector<Tensor> sparse_float_feature_values_list,
                    std::vector<Tensor> sparse_float_feature_shapes_list,
                    std::vector<Tensor> sparse_int_feature_indices_list,
                    std::vector<Tensor> sparse_int_feature_values_list,
                    std::vector<Tensor> sparse_int_feature_shapes_list);

  // Creates an example iterable for the requested slice.
  ExamplesIterable examples_iterable(int64 example_start,
                                     int64 example_end) const {
    QCHECK(example_start >= 0 && example_end >= 0);
    QCHECK(example_start < batch_size_ && example_end <= batch_size_);
    return ExamplesIterable(
        dense_float_feature_columns_, sparse_float_feature_columns_,
        sparse_int_feature_columns_, example_start, example_end);
  }

  // Returns the fixed batch size.
  int64 batch_size() const { return batch_size_; }

 private:
  // Total number of examples in the batch.
  const int64 batch_size_;

  // Dense float feature columns.
  std::vector<Tensor> dense_float_feature_columns_;

  // Sparse float feature columns.
  std::vector<sparse::SparseTensor> sparse_float_feature_columns_;

  // Sparse int feature columns.
  std::vector<sparse::SparseTensor> sparse_int_feature_columns_;
};

}  // namespace utils
}  // namespace boosted_trees
}  // namespace tensorflow

#endif  // THIRD_PARTY_TENSORFLOW_CONTRIB_BOOSTED_TREES_LIB_UTILS_BATCH_FEATURES_H_
