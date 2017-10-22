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
#include "tensorflow/core/kernels/captured_function.h"

#include <utility>

#include "tensorflow/core/common_runtime/threadpool_device.h"
#include "tensorflow/core/framework/allocator.h"
#include "tensorflow/core/framework/device_attributes.pb.h"
#include "tensorflow/core/framework/lookup_interface.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/queue_interface.h"
#include "tensorflow/core/framework/reader_interface.h"
#include "tensorflow/core/framework/resource_handle.pb_text.h"
#include "tensorflow/core/kernels/dataset.h"
#include "tensorflow/core/kernels/variable_ops.h"
#include "tensorflow/core/platform/notification.h"
#include "tensorflow/core/public/session_options.h"


namespace tensorflow {

/* static */
Status CapturedFunction::Create(
    OpKernelContext* ctx, const NameAttrList* func, int graph_def_version,
    std::vector<Tensor> captured_inputs,
    std::unique_ptr<CapturedFunction>* out_function) {
  // NOTE(mrry): We need to assign a name to the device, and we choose
  // the same name as the calling context's device so that we do not
  // need to rewrite resource handles that are found in `captured_inputs`.
  std::unique_ptr<Device> device(new ThreadPoolDevice(
      SessionOptions(), ctx->device()->attributes().name(), Bytes(256 << 20),
      DeviceLocality(), cpu_allocator()));

// TODO(mrry): Handle arbitrary resource types, which might require a
// redesign (or opening up access to `ResourceMgr::DoLookup()` and
// `ResourceMgr::DoCreate()` to this code).
#define HANDLE_RESOURCE_TYPE(ResourceType)                                     \
  if (input_handle.hash_code() == MakeTypeIndex<ResourceType>().hash_code()) { \
    ResourceType* resource;                                                    \
    Status s = LookupResource(ctx, input_handle, &resource);                   \
    if (errors::IsNotFound(s)) {                                               \
      return errors::FailedPrecondition(                                       \
          "Failed to capture resource named \"", input_handle.name(),          \
          "\" in a dataset function. You may need to initialize it "           \
          "explicitly before initializing an iterator that uses it.");         \
    } else if (!s.ok()) {                                                      \
      return s;                                                                \
    }                                                                          \
    TF_RETURN_IF_ERROR(device->resource_manager()->Create(                     \
        input_handle.container(), input_handle.name(), resource));             \
    continue;                                                                  \
  }

  for (size_t i = 0; i < captured_inputs.size(); ++i) {
    if (captured_inputs[i].dtype() == DT_RESOURCE) {
      // Extract the resource from `ctx->resource_manager()` and
      // insert it into `device->resource_manager()` so that it can be
      // used when the function executes.
      ResourceHandle input_handle =
          captured_inputs[i].scalar<ResourceHandle>()();
      HANDLE_RESOURCE_TYPE(lookup::LookupInterface);
      HANDLE_RESOURCE_TYPE(QueueInterface);
      HANDLE_RESOURCE_TYPE(Var);
      return errors::Unimplemented(
          "Cannot currently capture resource '",
          ProtoDebugString(input_handle),
          "' in a dataset function (type not supported).");
    }
  }
#undef HANDLE_RESOURCE_TYPE

  std::unique_ptr<FunctionLibraryDefinition> flib_def(
      new FunctionLibraryDefinition(
          *ctx->function_library()->GetFunctionLibraryDefinition()));
  std::unique_ptr<FunctionLibraryRuntime> lib(NewFunctionLibraryRuntime(
      nullptr /* device_mgr */, ctx->env(), device.get(), graph_def_version,
      flib_def.get(), {} /* TODO(mrry): OptimizerOptions? */));

  FunctionLibraryRuntime::Handle f_handle;
  TF_RETURN_IF_ERROR(lib->Instantiate(func->name(),
                                      func->attr(), &f_handle));

  out_function->reset(new CapturedFunction(
      std::move(device), std::move(flib_def), std::move(lib), f_handle,
      std::move(captured_inputs)));
  return Status::OK();
}

Status CapturedFunction::Run(FunctionLibraryRuntime::Options f_opts,
                             gtl::ArraySlice<Tensor> args,
                             std::vector<Tensor>* rets) {
  Notification n;
  Status s;
  auto done_callback = [&n, &s](Status func_status) {
    s.Update(func_status);
    n.Notify();
  };
  // TODO(mrry): Add cancellation manager support to IteratorContext
  // so that we can cancel running map functions. The local
  // cancellation manager here is created so that we can run kernels
  // (such as queue kernels) that depend on the non-nullness
  // `OpKernelContext::cancellation_manager()`, but additional effort
  // will be required to plumb it through the `IteratorContext`.
  CancellationManager c_mgr;
  f_opts.cancellation_manager = &c_mgr;
  // TODO(mrry): Implement a synchronous version of
  // FunctionLibraryRuntime::Run() that avoids a context switch for small
  // functions.
  if (captured_inputs_.empty()) {
    lib_->Run(f_opts, f_handle_, args, rets, done_callback);
  } else {
    std::vector<Tensor> args_with_captured;
    args_with_captured.reserve(args.size() + captured_inputs_.size());
    args_with_captured.insert(args_with_captured.end(), args.begin(),
                              args.end());
    args_with_captured.insert(args_with_captured.end(),
                              captured_inputs_.begin(), captured_inputs_.end());
    lib_->Run(f_opts, f_handle_, args_with_captured, rets, done_callback);
  }
  n.WaitForNotification();
  return s;
}

CapturedFunction::CapturedFunction(
    std::unique_ptr<Device> device,
    std::unique_ptr<FunctionLibraryDefinition> flib_def,
    std::unique_ptr<FunctionLibraryRuntime> lib,
    FunctionLibraryRuntime::Handle f_handle,
    std::vector<Tensor> captured_inputs)
    : device_(std::move(device)),
      flib_def_(std::move(flib_def)),
      lib_(std::move(lib)),
      f_handle_(f_handle),
      captured_inputs_(std::move(captured_inputs)) {}

}  // namespace tensorflow
