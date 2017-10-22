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

#ifdef TENSORFLOW_USE_VERBS

#include "tensorflow/contrib/verbs/rdma.h"
#include <cstdlib>
#include "tensorflow/contrib/verbs/verbs_util.h"
#include "tensorflow/core/common_runtime/device_mgr.h"
#include "tensorflow/core/common_runtime/dma_helper.h"
#include "tensorflow/core/common_runtime/gpu/gpu_util.h"
#include "tensorflow/core/distributed_runtime/rendezvous_mgr_interface.h"
#include "tensorflow/core/distributed_runtime/session_mgr.h"
#include "tensorflow/core/framework/rendezvous.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/hash/hash.h"
#include "tensorflow/core/lib/random/random.h"

namespace tensorflow {

namespace {
// hash name to 32-bit integer
uint32_t NameHash(const string& name) {
  return Hash32(name.data(), name.size(), 0x1234ABCD);
}

// convenience function for printing message
string MessageTypeToString(RdmaMessageType rmt) {
  switch (rmt) {
    case RDMA_MESSAGE_ACK:
      return "RDMA_MESSAGE_ACK";
      break;
    case RDMA_MESSAGE_BUFFER_IDLE:
      return "RDMA_MESSAGE_BUFFER_IDLE";
      break;
    case RDMA_MESSAGE_BUFFER_REQUEST:
      return "RDMA_MESSAGE_BUFFER_REQUEST";
      break;
    case RDMA_MESSAGE_BUFFER_RESPONSE:
      return "RDMA_MESSAGE_BUFFER_RESPONSE";
      break;
    case RDMA_MESSAGE_TENSOR_REQUEST:
      return "RDMA_MESSAGE_TENSOR_REQUEST";
      break;
    case RDMA_MESSAGE_TENSOR_WRITE:
      return "RDMA_MESSAGE_TENSOR_WRITE";
      break;
    default:
      return "UNKNOWN MESSAGE";
  }
}
}  // namespace

ibv_context* open_default_device() {
  ibv_device** dev_list;
  ibv_device* ib_dev;
  dev_list = ibv_get_device_list(NULL);
  CHECK(dev_list) << "No InfiniBand device found";
  ib_dev = dev_list[0];
  CHECK(ib_dev) << "No InfiniBand device found";
  ibv_context* context = ibv_open_device(ib_dev);
  CHECK(context) << "Open context failed for " << ibv_get_device_name(ib_dev);
  return context;
}

ibv_pd* alloc_protection_domain(ibv_context* context) {
  ibv_pd* pd = ibv_alloc_pd(context);
  CHECK(pd) << "Failed to allocate protection domain";
  return pd;
}

RdmaAdapter::RdmaAdapter(const WorkerEnv* worker_env)
    : context_(open_default_device()),
      pd_(alloc_protection_domain(context_)),
      worker_env_(worker_env) {
  event_channel_ = ibv_create_comp_channel(context_);
  CHECK(event_channel_) << "Failed to create completion channel";
  cq_ = ibv_create_cq(context_, MAX_CONCURRENT_WRITES * 2, NULL, event_channel_,
                      0);
  CHECK(cq_) << "Failed to create completion queue";
  CHECK(!ibv_req_notify_cq(cq_, 0)) << "Failed to request CQ notification";
  polling_thread_.reset(Env::Default()->StartThread(
      ThreadOptions(), "RdmaAdapterCQThread", [this] { Process_CQ(); }));
  VLOG(2) << "Start RdmaAdapter: " << name();
}

RdmaAdapter::~RdmaAdapter() {
  polling_thread_.reset();
  CHECK(!ibv_destroy_cq(cq_)) << "Failed to destroy CQ";
  CHECK(!ibv_destroy_comp_channel(event_channel_))
      << "Failed to destroy channel";
  CHECK(!ibv_dealloc_pd(pd_)) << "Failed to deallocate PD";
  CHECK(!ibv_close_device(context_)) << "Failed to release context";
}

string RdmaAdapter::name() const { return string(context_->device->name); }

// Function to process incoming messages
// There are two types of messages:
// 1. IBV_WC_RECV_RDMA_WITH_IMM (receive)
// 2. IBV_WC_RDMA_WRITE (send))
void RdmaAdapter::Process_CQ() {
  while (true) {
    ibv_cq* cq;
    void* cq_context;
    CHECK(!ibv_get_cq_event(event_channel_, &cq, &cq_context));
    CHECK(cq == cq_);
    ibv_ack_cq_events(cq, 1);
    CHECK(!ibv_req_notify_cq(cq_, 0));

    int ne =
        ibv_poll_cq(cq_, MAX_CONCURRENT_WRITES * 2, static_cast<ibv_wc*>(wc_));
    CHECK_GE(ne, 0);
    for (int i = 0; i < ne; ++i) {
      CHECK(wc_[i].status == IBV_WC_SUCCESS)
          << "Failed status \n"
          << ibv_wc_status_str(wc_[i].status) << " " << wc_[i].status << " "
          << static_cast<int>(wc_[i].wr_id) << " " << wc_[i].vendor_err;
      if (wc_[i].opcode == IBV_WC_RECV_RDMA_WITH_IMM) {
        RdmaChannel* rc = reinterpret_cast<RdmaChannel*>(wc_[i].wr_id);
        // put back a recv wr.
        rc->Recv();
        // imm_data is the index of RX buffer in the buffer table.
        uint32_t imm_data = wc_[i].imm_data;
        RdmaBuffer* rb = rc->FindBuffer(imm_data);
        RdmaMessage rm;
        RdmaMessage::ParseMessage(rm, rb->buffer_);
        VLOG(2) << "recv RDMA message: " << MessageTypeToString(rm.type_);

        if (rm.type_ == RDMA_MESSAGE_ACK) {
          // receive an ack to a message
          rb = rc->tx_message_buffer_;
          rb->SetBufferStatus(remote, idle);
          rb->SendNextItem();
        } else if (rm.type_ == RDMA_MESSAGE_TENSOR_REQUEST) {
          // received a request-for-tensor message
          // send ack to release remote tx message buffer
          RdmaBuffer* ab = rc->tx_ack_buffer_;
          ab->SendNextItem();
          // find or create buffer
          RdmaBuffer* tb = rc->FindOrCreateBuffer(rm.name_);
          string key_with_step_id =
              VerbsUtil::AppendStepidToKey(rm.name_, rm.step_id_);
          tb->EnqueueItem(key_with_step_id);
          // send the next tensor
          worker_env_->compute_pool->Schedule([tb]() { tb->SendNextItem(); });
        } else if (rm.type_ == RDMA_MESSAGE_BUFFER_IDLE) {
          // receive tensor-buffer-ready message
          // send ack to release remote tx message buffer
          RdmaBuffer* ab = rc->tx_ack_buffer_;
          ab->SendNextItem();
          // find buffer
          RdmaBuffer* tb = rc->FindBuffer(rm.name_);
          tb->SetBufferStatus(remote, idle);
          worker_env_->compute_pool->Schedule([tb]() { tb->SendNextItem(); });
        } else if (rm.type_ == RDMA_MESSAGE_BUFFER_REQUEST) {
          // remote host requests to create a tensor buffer;
          // send ack to release remote tx message buffer
          RdmaBuffer* ab = rc->tx_ack_buffer_;
          ab->SendNextItem();
          // find or create the buffer
          RdmaBuffer* tb = rc->FindOrCreateBuffer(rm.name_, TENSOR);
          RemoteMR rmr;
          rmr.remote_addr = rm.remote_addr_;
          rmr.rkey = rm.rkey_;
          tb->SetRemoteMR(rmr, true);
          tb->CreateCPUBuffer(rm.buffer_size_);
          // create RDMA_MESSAGE_BUFFER_RESPONSE message
          RdmaMessage br;
          br.type_ = RDMA_MESSAGE_BUFFER_RESPONSE;
          br.name_size_ = rm.name_.size();
          br.name_ = rm.name_;
          br.buffer_size_ = rm.buffer_size_;
          br.remote_addr_ = reinterpret_cast<uint64_t>(tb->buffer_);
          br.rkey_ = tb->self_->rkey;
          string message = RdmaMessage::CreateMessage(br);
          RdmaBuffer* mb = rc->tx_message_buffer_;
          mb->EnqueueItem(message);
          mb->SendNextItem();
        } else if (rm.type_ == RDMA_MESSAGE_BUFFER_RESPONSE) {
          // remote creates a buffer and responds
          // send ack to release remote tx message buffer
          RdmaBuffer* ab = rc->tx_ack_buffer_;
          ab->SendNextItem();
          // find buffer
          RdmaBuffer* tb = rc->FindBuffer(rm.name_);
          CHECK(rm.buffer_size_ == tb->size_)
              << "rm.buffer_size = " << rm.buffer_size_
              << "tb->size_ = " << tb->size_ << "rm.name_ = " << rm.name_;
          RemoteMR rmr;
          rmr.remote_addr = rm.remote_addr_;
          rmr.rkey = rm.rkey_;
          tb->SetRemoteMR(rmr, true);
          tb->SetBufferStatus(local, idle);
          tb->SetBufferStatus(remote, idle);
          worker_env_->compute_pool->Schedule([tb]() { tb->SendNextItem(); });
        } else if (rm.type_ == RDMA_MESSAGE_TENSOR_WRITE) {
          // tensor RDMA write completed
          worker_env_->compute_pool->Schedule([rm, rc]() {
            string key_with_step_id =
                VerbsUtil::AppendStepidToKey(rm.name_, rm.step_id_);
            rc->RunRecvCallback(key_with_step_id);
          });
        }
      } else if (wc_[i].opcode == IBV_WC_RDMA_WRITE) {
        RdmaBuffer* rb = reinterpret_cast<RdmaBuffer*>(wc_[i].wr_id);
        rb->SetBufferStatus(local, idle);
        RdmaMessage rm;
        RdmaMessage::ParseMessage(rm, rb->buffer_);
        VLOG(2) << "sent RDMA message: " << MessageTypeToString(rm.type_);
        if (rm.type_ != RDMA_MESSAGE_ACK) {
          worker_env_->compute_pool->Schedule([rb]() { rb->SendNextItem(); });
        }
      }
    }
  }
}

RdmaChannel::RdmaChannel(const RdmaAdapter* adapter, const string local_name,
                         const string remote_name)
    : adapter_(adapter), local_name_(local_name), remote_name_(remote_name) {
  // Create queue pair
  {
    struct ibv_qp_init_attr attr;
    memset(&attr, 0, sizeof(ibv_qp_init_attr));
    attr.send_cq = adapter_->cq_;
    attr.recv_cq = adapter_->cq_;
    attr.cap.max_send_wr = RdmaAdapter::MAX_CONCURRENT_WRITES;
    attr.cap.max_recv_wr = RdmaAdapter::MAX_CONCURRENT_WRITES;
    attr.cap.max_send_sge = 1;
    attr.cap.max_recv_sge = 1;
    attr.qp_type = IBV_QPT_RC;

    qp_ = ibv_create_qp(adapter_->pd_, &attr);
    CHECK(qp_) << "Failed to create queue pair";
  }

  // Init queue pair
  {
    struct ibv_qp_attr attr;
    memset(&attr, 0, sizeof(ibv_qp_attr));
    attr.qp_state = IBV_QPS_INIT;
    attr.pkey_index = 0;
    attr.port_num = 1;
    attr.qp_access_flags = IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE;

    int mask =
        IBV_QP_STATE | IBV_QP_PKEY_INDEX | IBV_QP_PORT | IBV_QP_ACCESS_FLAGS;
    CHECK(!ibv_modify_qp(qp_, &attr, mask)) << "Failed to set QP to INIT";
  }

  // Local address
  {
    struct ibv_port_attr attr;
    CHECK(!ibv_query_port(adapter_->context_, (uint8_t)1, &attr))
        << "Query port";
    self_.lid = attr.lid;
    self_.qpn = qp_->qp_num;
    self_.psn = static_cast<uint32_t>(random::New64()) & 0xffffff;
    union ibv_gid gid;
    CHECK(!ibv_query_gid(adapter_->context_, (uint8_t)1, 0, &gid))
        << "Query gid";
    self_.snp = gid.global.subnet_prefix;
    self_.iid = gid.global.interface_id;
  }

  // create message and ack buffers, then initialize the tables.
  {
    const string buffer_names[] = {"tx_message_buffer", "rx_message_buffer",
                                   "tx_ack_buffer", "rx_ack_buffer"};
    tx_message_buffer_ = new RdmaMessageBuffer(this, buffer_names[0]);
    rx_message_buffer_ = new RdmaMessageBuffer(this, buffer_names[1]);
    tx_ack_buffer_ = new RdmaAckBuffer(this, buffer_names[2]);
    rx_ack_buffer_ = new RdmaAckBuffer(this, buffer_names[3]);
    message_buffers_.reserve(kNumMessageBuffers);
    message_buffers_.push_back(tx_message_buffer_);
    message_buffers_.push_back(rx_message_buffer_);
    message_buffers_.push_back(tx_ack_buffer_);
    message_buffers_.push_back(rx_ack_buffer_);
    // create buffer on host
    tx_message_buffer_->CreateCPUBuffer(RdmaMessage::kRdmaMessageBufferSize);
    rx_message_buffer_->CreateCPUBuffer(RdmaMessage::kRdmaMessageBufferSize);
    tx_ack_buffer_->CreateCPUBuffer(RdmaMessage::kRdmaAckBufferSize);
    rx_ack_buffer_->CreateCPUBuffer(RdmaMessage::kRdmaAckBufferSize);
    // bt_mu_.lock() is not used in constructor.
    for (int i = 0; i < kNumMessageBuffers; i++) {
      uint32_t index = NameHash(buffer_names[i]);
      buffer_table_.insert({index, message_buffers_[i]});
      buffer_index_name_table_.insert({index, buffer_names[i]});
      buffer_name_index_table_.insert({buffer_names[i], index});
    }

    // Initiate recv
    for (int i = 0; i < 100; i++) {
      Recv();
    }
  }
}

RdmaChannel::~RdmaChannel() {
  CHECK(!ibv_destroy_qp(qp_)) << "Failed to destroy QP";
  delete tx_message_buffer_;
  delete rx_message_buffer_;
  delete tx_ack_buffer_;
  delete rx_ack_buffer_;
}

void RdmaChannel::SetRemoteAddress(const RdmaAddress& ra, bool override) {
  mutex_lock lock{mu_};
  if ((override) || (!remote_set_)) {
    remote_.lid = ra.lid;
    remote_.qpn = ra.qpn;
    remote_.psn = ra.psn;
    remote_.snp = ra.snp;
    remote_.iid = ra.iid;
    remote_set_ = true;
  } else {
    CHECK(remote_.lid == ra.lid);
    CHECK(remote_.qpn == ra.qpn);
    CHECK(remote_.psn == ra.psn);
    CHECK(remote_.snp == ra.snp);
    CHECK(remote_.iid == ra.iid);
  }
}

// Adding tokens to the completion queue
// Tokens are needed to process future messages.
void RdmaChannel::Recv() {
  struct ibv_recv_wr wr;
  memset(&wr, 0, sizeof(wr));
  wr.wr_id = (uint64_t)this;
  struct ibv_recv_wr* bad_wr;
  CHECK(!ibv_post_recv(qp_, &wr, &bad_wr)) << "Failed to post recv";
}

// Lookup 32-bit buffer index from buffer name
// Args:
//   buffer_name: name of the buffer
// Returns:
//   32-bit index
uint32_t RdmaChannel::LookupBufferIndex(const string& buffer_name) {
  mutex_lock lock{bt_mu_};
  BufferNameIndexTable::iterator iter =
      buffer_name_index_table_.find(buffer_name);
  CHECK(iter != buffer_name_index_table_.end());
  return iter->second;
}

// Find a buffer by its 32-bit index
// Args:
//   index: 32-bit hash code of the tensor buffer name
// Returns:
//   name of the tensor buffer
RdmaBuffer* RdmaChannel::FindBuffer(const uint32_t index) {
  mutex_lock lock{bt_mu_};
  BufferTable::iterator iter = buffer_table_.find(index);
  CHECK(iter != buffer_table_.end());
  return iter->second;
}

// Find a buffer by its name
// Args:
//   name: name of the buffer
// Returns:
//   the named rdma buffer
RdmaBuffer* RdmaChannel::FindBuffer(const string& name) {
  uint32_t index = LookupBufferIndex(name);
  return FindBuffer(index);
}

// Find a buffer if it exists, otherwise create one.
// The memory inside the created buffer is not allocated.
// Args:
//   name: the name of the buffer
//   buffer_type: TENSOR, MESSAGE or ACK.
// Returns:
//   the named buffer
RdmaBuffer* RdmaChannel::FindOrCreateBuffer(const string& name,
                                            BufferType buffer_type) {
  mutex_lock lock{bt_mu_};
  RdmaBuffer* rb;
  // find index
  BufferNameIndexTable::iterator iter = buffer_name_index_table_.find(name);
  if (iter != buffer_name_index_table_.end()) {
    uint32_t index = iter->second;
    // find buffer
    BufferTable::iterator iter = buffer_table_.find(index);
    CHECK(iter != buffer_table_.end());
    rb = iter->second;
  } else {
    uint32_t index = NameHash(name);
    if (buffer_type == TENSOR) {
      rb = new RdmaTensorBuffer(this, name);
    } else if (buffer_type == MESSAGE) {
      rb = new RdmaMessageBuffer(this, name);
    } else if (buffer_type == ACK) {
      rb = new RdmaAckBuffer(this, name);
    }
    buffer_name_index_table_.insert({name, index});
    buffer_index_name_table_.insert({index, name});
    buffer_table_.insert({index, rb});
  }
  CHECK(rb);
  return rb;
}

// Insert callback to the callback_table.
// The callback is activated when the corresponding tensor is received.
// Arg:
//   key: the name of the tensor
//   recv_done: the callback associated with the tensor.
// Returns:
//   None
void RdmaChannel::InsertRecvCallback(const string& key,
                                     std::function<void()> recv_done) {
  mutex_lock lock{ct_mu_};
  callback_table_.insert({key, recv_done});
}

// Remove callback from the callback_table.
// Arg:
//   key: the name of the tensor
// Returns:
//   None
void RdmaChannel::RemoveRecvCallback(const string& key) {
  mutex_lock lock{ct_mu_};
  callback_table_.erase(key);
}

// Run named callback in the callback_table.
// Arg:
//   key: the name of the tensor
// Returns:
//   None
void RdmaChannel::RunRecvCallback(const string& key) {
  std::function<void()> recv_done;
  {
    mutex_lock lock{ct_mu_};
    CallbackTable::iterator iter = callback_table_.find(key);
    CHECK(iter != callback_table_.end());
    recv_done = iter->second;
  }
  recv_done();
}

void RdmaChannel::Connect() {
  {
    mutex_lock lock{mu_};
    CHECK(remote_set_) << "remote channel is not set";
  }
  Connect(remote_);
}

// Setup channel to a remote node
// Args:
//   remoteAddr: the rdma address of a remote channel.
// Returns:
//   None
void RdmaChannel::Connect(const RdmaAddress& remoteAddr) {
  mutex_lock lock{mu_};
  if (!connected_) {
    struct ibv_qp_attr attr;
    memset(&attr, 0, sizeof(ibv_qp_attr));
    attr.qp_state = IBV_QPS_RTR;
    struct ibv_port_attr port_attr;
    CHECK(!ibv_query_port(adapter_->context_, (uint8_t)1, &port_attr))
        << "Query port failed";
    // This assumes both QP's ports are configured with the same MTU
    attr.path_mtu = port_attr.active_mtu;
    attr.dest_qp_num = remoteAddr.qpn;
    attr.rq_psn = remoteAddr.psn;
    attr.max_dest_rd_atomic = 1;
    attr.min_rnr_timer = 12;
    attr.ah_attr.is_global = 1;
    attr.ah_attr.grh.dgid.global.subnet_prefix = remoteAddr.snp;
    attr.ah_attr.grh.dgid.global.interface_id = remoteAddr.iid;
    attr.ah_attr.grh.flow_label = 0;
    attr.ah_attr.grh.hop_limit = 255;
    attr.ah_attr.dlid = remoteAddr.lid;
    attr.ah_attr.sl = 0;
    attr.ah_attr.src_path_bits = 0;
    attr.ah_attr.port_num = 1;

    int r;
    CHECK(!(r = ibv_modify_qp(qp_, &attr,
                              IBV_QP_STATE | IBV_QP_AV | IBV_QP_PATH_MTU |
                                  IBV_QP_DEST_QPN | IBV_QP_RQ_PSN |
                                  IBV_QP_MAX_DEST_RD_ATOMIC |
                                  IBV_QP_MIN_RNR_TIMER)))
        << "QP to Ready to Receive " << r;

    memset(&attr, 0, sizeof(ibv_qp_attr));
    attr.qp_state = IBV_QPS_RTS;
    attr.sq_psn = self_.psn;
    attr.timeout = 14;
    attr.retry_cnt = 7;
    attr.rnr_retry = 7; /* infinite */
    attr.max_rd_atomic = 1;

    CHECK(!(r = ibv_modify_qp(qp_, &attr,
                              IBV_QP_STATE | IBV_QP_TIMEOUT | IBV_QP_RETRY_CNT |
                                  IBV_QP_RNR_RETRY | IBV_QP_SQ_PSN |
                                  IBV_QP_MAX_QP_RD_ATOMIC)))
        << "QP to Ready to Send " << r;

    connected_ = true;
  } else {
    LOG(INFO) << "channel already connected";
  }
}

RdmaBuffer::RdmaBuffer(RdmaChannel* channel, string name)
    : channel_(channel), name_(name) {}

RdmaBuffer::~RdmaBuffer() {
  CHECK(!ibv_dereg_mr(self_)) << "ibv_dereg_mr failed";
  FreeBuffer();
}

void RdmaBuffer::FreeBuffer() {
  if ((buffer_ != nullptr) && buffer_on_host_) {
    free(buffer_);
  }
  // TODO
  // release buffer if it is on device.
  // We don't support RDMABuffer on device at this moment.
}

// Allocate CPU memory for the Rdma buffer
// Args:
//   size: to-be-allocated memory size
//   lock: whether or not mutex_lock the process to protect concurrency.
// Returns:
//   None
void RdmaBuffer::CreateCPUBuffer(size_t size, bool lock) {
  CHECK(size > 0);
  if (lock) {
    mu_.lock();
  }
  if (local_status_ != none) {
    // delete existing buffer
    CHECK(!ibv_dereg_mr(self_)) << "ibv_dereg_mr failed";
    FreeBuffer();
  }
  size_ = size;
  buffer_ = malloc(size_);
  self_ = ibv_reg_mr(channel_->adapter_->pd_, buffer_, size_,
                     IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE);
  CHECK(self_) << "Failed to register memory region";
  buffer_on_host_ = true;
  local_status_ = idle;
  if (lock) {
    mu_.unlock();
  }
}

// Set address of remote memory region
// Args:
//   rmr: address of remote memory region
//   override: whether override existing information
// Returns:
//   None
void RdmaBuffer::SetRemoteMR(RemoteMR rmr, bool override) {
  mutex_lock lock{mu_};
  if ((override) || (remote_status_ == none)) {
    remote_.remote_addr = rmr.remote_addr;
    remote_.rkey = rmr.rkey;
    remote_status_ = idle;
  } else {
    CHECK(remote_.remote_addr == rmr.remote_addr);
    CHECK(remote_.rkey == rmr.rkey);
  }
}

// Put a task in the buffer's job queue
void RdmaBuffer::EnqueueItem(string item) {
  mutex_lock lock{mu_};
  queue_.push(item);
}

// Rdma-Write the content of the buffer
void RdmaBuffer::Write(uint32_t imm_data, size_t buffer_size) {
  struct ibv_sge list;
  list.addr = (uint64_t)buffer_;
  list.length = buffer_size;
  list.lkey = self_->lkey;

  struct ibv_send_wr wr;
  memset(&wr, 0, sizeof(wr));
  wr.wr_id = (uint64_t)this;
  wr.sg_list = &list;
  wr.num_sge = 1;
  wr.opcode = IBV_WR_RDMA_WRITE_WITH_IMM;
  wr.send_flags = IBV_SEND_SIGNALED;
  wr.imm_data = imm_data;
  wr.wr.rdma.remote_addr = (uint64_t)remote_.remote_addr;
  wr.wr.rdma.rkey = remote_.rkey;

  struct ibv_send_wr* bad_wr;
  CHECK(!ibv_post_send(channel_->qp_, &wr, &bad_wr)) << "Failed to post send";
}

RdmaAckBuffer::RdmaAckBuffer(RdmaChannel* channel, string name)
    : RdmaBuffer(channel, name) {}

RdmaMessageBuffer::RdmaMessageBuffer(RdmaChannel* channel, string name)
    : RdmaBuffer(channel, name) {}

RdmaTensorBuffer::RdmaTensorBuffer(RdmaChannel* channel, string name)
    : RdmaBuffer(channel, name) {}

// Send the next ack from the buffer's job queue.
void RdmaAckBuffer::SendNextItem() {
  uint32_t imm_data = LookupBufferIndex("rx_ack_buffer");
  RdmaMessage rm;
  rm.name_ = "rx_ack_buffer";
  rm.type_ = RDMA_MESSAGE_ACK;
  rm.name_size_ = rm.name_.size();
  string message = RdmaMessage::CreateMessage(rm);
  memcpy(buffer_, message.data(), message.size());
  Write(imm_data, message.size());
}

// Send the next message from the buffer's job queue.
void RdmaMessageBuffer::SendNextItem() {
  uint32_t imm_data = LookupBufferIndex("rx_message_buffer");
  mu_.lock();
  if (!queue_.empty() && (local_status_ == idle) && (remote_status_ == idle)) {
    local_status_ = busy;
    remote_status_ = busy;
    string message = queue_.front();
    queue_.pop();
    // local/remote_status_ won't be set back to idle
    // unitl Write() is successful
    mu_.unlock();
    memcpy(buffer_, message.data(), message.size());
    Write(imm_data, message.size());
  } else {
    mu_.unlock();
  }
}

// Send the next tensor from the buffer's job queue.
void RdmaTensorBuffer::SendNextItem() {
  // get the key
  string key_with_step_id = "";
  {
    mutex_lock lock{mu_};
    if (!queue_.empty()) {
      key_with_step_id = queue_.front();
      queue_.pop();
    }
  }
  // send the tensor if a key is acquired.
  if (key_with_step_id != "") {
    VLOG(2) << "try to send tensor: " << key_with_step_id;
    string key;
    int64 step_id;
    VerbsUtil::GetKeyAndStepId(key_with_step_id, key, step_id);
    CHECK(key.compare(name_) == 0);
    Rendezvous::ParsedKey parsed;
    Rendezvous::ParseKey(key, &parsed);
    Rendezvous::DoneCallback cb = [this, key_with_step_id, key, step_id,
                                   parsed](const Status& status,
                                           const Rendezvous::Args& send_args,
                                           const Rendezvous::Args& recv_args,
                                           const Tensor& in, bool is_dead) {
      CHECK(status.ok()) << "RecvLocalAsync was not ok, key" << key_with_step_id
                         << " error message: " << status.error_message();
      size_t buffer_size = RdmaMessage::kMessageTotalBytes;
      size_t tensor_bytes = 0;
      TensorProto proto;
      // Figures out which device the tensor is hosted on.
      Device* src_dev = nullptr;
      Status s = channel_->adapter_->worker_env_->device_mgr->LookupDevice(
          parsed.src_device, &src_dev);
      CHECK(s.ok()) << "src device not found";
      // Does the device have the right incarnation number we expect?
      CHECK(src_dev->attributes().incarnation() == parsed.src_incarnation)
          << "RecvTensor expects a different device incarnation: "
          << parsed.src_incarnation << " vs. "
          << src_dev->attributes().incarnation()
          << ". Your worker job was probably restarted. Check your "
          << "worker job for the reason why it was restarted.";
      Device* dst_dev = nullptr;
      // destination is on CPU.
      s = channel_->adapter_->worker_env_->device_mgr->LookupDevice("CPU:0",
                                                                    &dst_dev);
      CHECK(s.ok()) << "dst device not found";
      AllocatorAttributes dst_alloc_attr;
      dst_alloc_attr.set_on_host(true);
      // string tensor needs to be serialized
      if (src_dev->tensorflow_gpu_device_info() &&
          (!send_args.alloc_attrs.on_host())) {
        CHECK(send_args.device_context)
            << "send dev name: " << src_dev->name()
            << " gpu_info: " << src_dev->tensorflow_gpu_device_info();
        // "val" is on a GPU. Uses GPUUtil to fill the proto.
        s = VerbsUtil::SetProtoFromGPUSync(
            in, src_dev, send_args.device_context, &proto, is_dead);
        CHECK(s.ok()) << "set proto from gpu sync";
      } else {
        // tensor is in CPU memory.
        in.AsProtoTensorContent(&proto);
      }
      tensor_bytes = proto.ByteSize();
      // maybe some margin for string tensor?
      buffer_size += tensor_bytes;
      // prepare message
      RdmaMessage rm;
      rm.name_size_ = key.size();
      rm.name_ = key;
      rm.tensor_shape_ = in.shape();
      rm.data_type_ = in.dtype();
      rm.step_id_ = step_id;
      rm.is_dead_ = is_dead;
      rm.tensor_bytes_ = tensor_bytes;
      rm.buffer_size_ = buffer_size;
      mu_.lock();
      if (local_status_ == none ||
          (buffer_size > size_ && local_status_ == idle &&
           remote_status_ == idle)) {
        if ((local_status_ != none) && (buffer_size > size_)) {
          CHECK(rm.data_type_ == DT_STRING)
              << "Only string tensor allows to change size";
        }
        CreateCPUBuffer(buffer_size, false);
        mu_.unlock();
        // put back the key since it is not sent;
        EnqueueItem(key_with_step_id);
        // ask the remote to create the same buffer
        rm.type_ = RDMA_MESSAGE_BUFFER_REQUEST;
        rm.remote_addr_ = reinterpret_cast<uint64_t>(buffer_);
        rm.rkey_ = self_->rkey;
        string message = RdmaMessage::CreateMessage(rm);
        channel_->tx_message_buffer_->EnqueueItem(message);
        channel_->tx_message_buffer_->SendNextItem();
      } else if ((local_status_ == idle) && (remote_status_ == idle)) {
        // both buffers are ready, send the tensor
        local_status_ = busy;
        remote_status_ = busy;
        // local/remote_status_ won't be set back to idle
        // unitl Write() is successful
        mu_.unlock();
        CHECK((buffer_size == size_ && rm.data_type_ != DT_STRING) ||
              (buffer_size <= size_ && rm.data_type_ == DT_STRING))
            << "tensor and buffer size do not agree!"
            << " buffer_size = " << size_
            << " requested tensor size = " << buffer_size << in.DebugString();
        uint32_t imm_data = LookupBufferIndex(key);
        rm.type_ = RDMA_MESSAGE_TENSOR_WRITE;
        string message = RdmaMessage::CreateMessage(rm);
        memcpy(buffer_, message.data(), message.size());
        if (!is_dead) {
          // copy the tensor buffer content
          void* output =
              static_cast<void*>(static_cast<char*>(buffer_) +
                                 RdmaMessage::kTensorBufferStartIndex);
          CHECK(tensor_bytes + RdmaMessage::kTensorBufferStartIndex <= size_);
          proto.SerializeToArray(output, tensor_bytes);
        } else {
          buffer_size = RdmaMessage::kMessageTotalBytes;
        }
        Write(imm_data, buffer_size);
      } else {
        mu_.unlock();
        // put back the key since it is not sent;
        EnqueueItem(key_with_step_id);
      }
    };
    channel_->adapter_->worker_env_->rendezvous_mgr
        ->RecvLocalAsync(step_id, parsed, cb);
  }
}

// Create a RdmaMessage according to the pre-defined format
// Args:
//   rm: the message structure
// Returns:
//   message in string format
string RdmaMessage::CreateMessage(const RdmaMessage& rm) {
  // Rdma Message format
  // type|name_size|name|step_id|buffer_size|remote_addr|rkey|is_dead|...
  //   1B|    2B   | 512|  8B   |    8B     |       8B  | 4B |    1B |...
  // ...|data_type|tensor_shape|tensor_bytes|tensor_buffer
  // ...|   XB    |    XB      |    8B      |...
  //
  // ACK:             type|13|"rx_ack_buffer"
  // TENSOR_REQUEST:  type|name_size|tensor_name|step_id
  // TENSOR_WRITE:    type|name_size|tensor_name|step_id|...|is_dead
  //                 |data_type|tensor_shape|tensor_bytes
  // BUFFER_IDLE:     type|name_size|buffer_name
  // BUFFER_REQUEST:
  // type|name_size|buffer_name|...|buffer_size|remote_addr|rkey|
  // BUFFER_RESPONSE:
  // type|name_size|buffer_name|...|buffer_size|remote_addr|rkey|
  char message[kMessageTotalBytes];
  // type
  message[kTypeStartIndex] = static_cast<char>(rm.type_) & 0xff;
  // size of name
  memcpy(&message[kNameSizeStartIndex], &rm.name_size_, sizeof(rm.name_size_));
  // name
  memcpy(&message[kNameStartIndex], rm.name_.data(), rm.name_.size());
  // buffer_size, remote_addr, rkey
  if ((rm.type_ == RDMA_MESSAGE_BUFFER_REQUEST) ||
      (rm.type_ == RDMA_MESSAGE_BUFFER_RESPONSE)) {
    memcpy(&message[kBufferSizeStartIndex], &rm.buffer_size_,
           sizeof(rm.buffer_size_));
    memcpy(&message[kRemoteAddrStartIndex], &rm.remote_addr_,
           sizeof(rm.remote_addr_));
    memcpy(&message[kRkeyStartIndex], &rm.rkey_, sizeof(rm.rkey_));
  }
  // step_id
  if ((rm.type_ == RDMA_MESSAGE_TENSOR_WRITE) ||
      (rm.type_ == RDMA_MESSAGE_TENSOR_REQUEST)) {
    memcpy(&message[kStepIdStartIndex], &rm.step_id_, sizeof(rm.step_id_));
  }
  // is_dead, data_type, tensor_shape, tensor_bytes
  if (rm.type_ == RDMA_MESSAGE_TENSOR_WRITE) {
    memcpy(&message[kIsDeadStartIndex], &rm.is_dead_, sizeof(rm.is_dead_));

    memcpy(&message[kDataTypeStartIndex], &rm.data_type_,
           sizeof(rm.data_type_));
    memcpy(&message[kTensorShapeStartIndex], &rm.tensor_shape_,
           sizeof(rm.tensor_shape_));
    memcpy(&message[kTensorBytesStartIndex], &rm.tensor_bytes_,
           sizeof(rm.tensor_bytes_));
  }
  return string(message, kMessageTotalBytes);
}

// Parse a RdmaMessage according to the pre-defined format
// Args:
//   rm: the message structure where the parsed message will be saved
//   buffer: the place where the raw message is stored
// Returns:
//   None
void RdmaMessage::ParseMessage(RdmaMessage& rm, void* buffer) {
  char* message = static_cast<char*>(buffer);
  // type
  rm.type_ = static_cast<RdmaMessageType>(message[kTypeStartIndex]);
  // name_size_
  memcpy(&rm.name_size_, &message[kNameSizeStartIndex], sizeof(rm.name_size_));
  // name
  rm.name_ = string(&message[kNameStartIndex], rm.name_size_);
  // buffer_size, remote_addr, rkey
  if ((rm.type_ == RDMA_MESSAGE_BUFFER_REQUEST) ||
      (rm.type_ == RDMA_MESSAGE_BUFFER_RESPONSE)) {
    memcpy(&rm.buffer_size_, &message[kBufferSizeStartIndex],
           sizeof(rm.buffer_size_));
    memcpy(&rm.remote_addr_, &message[kRemoteAddrStartIndex],
           sizeof(rm.remote_addr_));
    memcpy(&rm.rkey_, &message[kRkeyStartIndex], sizeof(rm.rkey_));
  }
  // step_id
  if ((rm.type_ == RDMA_MESSAGE_TENSOR_WRITE) ||
      (rm.type_ == RDMA_MESSAGE_TENSOR_REQUEST)) {
    memcpy(&rm.step_id_, &message[kStepIdStartIndex], sizeof(rm.step_id_));
  }
  // data_type, tensor_bytes, tensor_shape, is_dead
  if (rm.type_ == RDMA_MESSAGE_TENSOR_WRITE) {
    memcpy(&rm.is_dead_, &message[kIsDeadStartIndex], sizeof(rm.is_dead_));
    memcpy(&rm.data_type_, &message[kDataTypeStartIndex],
           sizeof(rm.data_type_));
    memcpy(&rm.tensor_shape_, &message[kTensorShapeStartIndex],
           sizeof(rm.tensor_shape_));
    memcpy(&rm.tensor_bytes_, &message[kTensorBytesStartIndex],
           sizeof(rm.tensor_bytes_));
  }
}

}  // end namespace tensorflow

#endif
