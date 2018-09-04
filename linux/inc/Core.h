/* Copyright 2017-2018 All Rights Reserved.
 *  Gyeonghwan Hong (redcarrottt@gmail.com)
 *  Eunsoo Park (esevan.park@gmail.com)
 *  Injung Hwang (sinban04@gmail.com)
 *
 * [Contact]
 *  Gyeonghwan Hong (redcarrottt@gmail.com)
 *
 * Licensed under the Apache License, Version 2.0(the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INC_COMMUNICATOR_H_
#define INC_COMMUNICATOR_H_

#include <SegmentManager.h>
#include <ServerAdapter.h>
#include <APIInternal.h>
#include <ArrivalTimeCounter.h>

#include <mutex>
#include <stdint.h>
#include <vector>

namespace sc {

enum CommErr {
  kProtOk = 0,
  kProtErr = -1,
};

/*
 * Control Request Code
 * It is used to classify "control request message" that is transferred to the
 * peer.
 *  - Commands: "Connect", "Disconnect"
 *  - Acks: "Ok", "Fail"
 *  - Private Data: "Priv"
 */
typedef enum {
  kCtrlReqConnect = 1,
  kCtrlReqSleep = 2,
  kCtrlReqWakeUp = 3,
  kCtrlReqPriv = 10,
} CtrlReq;

/*
 * Core State
 */
typedef enum {
  kCMStateIdle = 0,
  kCMStateStarting = 1,
  kCMStateReady = 2,
  kCMStateConnecting = 3, // Deprecated: this state is managed by network switcher.
  kCMStateDisconnecting = 4, // Deprecated: this state is managed by network switcher.
  kCMStateStopping = 5
} CMState;

/*
 * Transactions
 * Series of asynchronous callbacks, especially used for
 * connection/disconnection callbacks.
 */
class Core;
class StartCoreTransaction {
public:
  static bool run(Core *caller);
  bool start();
  static void connect_control_adapter_callback(bool is_success);
  static void connect_first_data_adapter_callback(bool is_success);

private:
  void done(bool is_success);

  StartCoreTransaction(Core *caller) {
    this->mCaller = caller;
  }
  static StartCoreTransaction *sOngoing;

  Core *mCaller;
};

class StopCoreTransaction {
public:
  static bool run(Core *caller);
  bool start();
  static void disconnect_control_adapter_callback(bool is_success);
  static void disconnect_data_adapter_callback(bool is_success);

private:
  void done(bool is_success); 
  
  StopCoreTransaction(Core *caller) {
    this->mCaller = caller;
  }
  static StopCoreTransaction *sOngoing;

  Core *mCaller;
  int mDataAdaptersCount;
  std::mutex mDataAdaptersCountLock;
};

class ControlMessageListener {
public:
  virtual void on_receive_control_message(int adapter_id, void *data,
                                          size_t len) = 0;
};

class ServerAdapter;

/*
 * Core
 */
class Core {
public:
  /*
   * APIs
   * These functions are mapped to ones in API.h
   */
  friend StartCoreTransaction;
  friend StopCoreTransaction;
  void start();
  void done_start(bool is_success);
  void stop();
  void done_stop(bool is_success);

  void register_control_adapter(ServerAdapter *adapter);
  void register_data_adapter(ServerAdapter *adapter);

  int send(const void *dataBuffer, uint32_t dataLength);
  int receive(void **pDataBuffer);

  CMState get_state(void) {
    std::unique_lock<std::mutex> lck(this->mStateLock);
    return this->mState;
  }

  /* Handling adapters */
  ServerAdapter *get_data_adapter(int index) {
    std::unique_lock<std::mutex> lck(this->mDataAdaptersLock);
    return this->mDataAdapters.at(index);
  }
  ServerAdapter *find_data_adapter_by_id(int adapter_id) {
    std::unique_lock<std::mutex> lck(this->mDataAdaptersLock);
    for (std::vector<ServerAdapter *>::iterator it =
             this->mDataAdapters.begin();
         it != this->mDataAdapters.end(); it++) {
      ServerAdapter *adapter = *it;
      if (adapter->get_id() == adapter_id) {
        return adapter;
      }
    }
    return NULL;
  }
  ServerAdapter *get_control_adapter() { return this->mControlAdapter; }
  int get_data_adapter_count(void) { return this->mDataAdapters.size(); }

  int get_total_bandwidth(void) {
    int num_adapters = 0;
    int now_total_bandwidth = 0;
    /* Statistics from control adapter */
    {
      ServerAdapter *adapter = this->get_control_adapter();
      int bandwidth_up = adapter->get_bandwidth_up();
      int bandwidth_down = adapter->get_bandwidth_down();
      now_total_bandwidth += (bandwidth_up + bandwidth_down);
      num_adapters++;
    }

    /* Statistics from data adapters */
    int data_adapter_count = this->get_data_adapter_count();
    for (int i = 0; i < data_adapter_count; i++) {
      ServerAdapter *adapter = this->get_data_adapter(i);
      if (adapter == NULL)
        continue;
      int bandwidth_up = adapter->get_bandwidth_up();
      int bandwidth_down = adapter->get_bandwidth_down();
      now_total_bandwidth += (bandwidth_up + bandwidth_down);
      num_adapters++;
    }
    return now_total_bandwidth;
  }

  /* Control message handling */
private:
  void send_control_message(const void *dataBuffer, size_t dataLength);

public:
  void send_request_connect(uint16_t adapter_id);
  void send_request_sleep(uint16_t adapter_id);
  void send_request_wake_up(uint16_t adapter_id);
  void send_noti_private_data(uint16_t adapter_id, char *private_data_buf,
                              uint32_t private_data_len);  
private:
  void send_request(CtrlReq request_code, uint16_t adapter_id);
public:
  void add_control_message_listener(ControlMessageListener *listener) {
    this->mControlMessageListeners.push_back(listener);
  }
  static void receive_control_message_loop(ServerAdapter *adapter);

  /* Get statistics */
  float get_ema_send_request_size() {
    return this->mSendRequestSize.get_em_average();
  }

  float get_ema_send_arrival_time() {
    return this->mSendArrivalTime.get_em_average();
  }

  /* Singleton */
  static Core *get_instance(void) {
    if (singleton == NULL) {
      singleton = new Core();
    }
    return singleton;
  }

  ~Core() { SegmentManager::get_instance()->free_segment_all(); }

private:
  /* Main function to switch adapters */
  void set_state(CMState new_state) {
    std::unique_lock<std::mutex> lck(this->mStateLock);
    this->mState = new_state;
  }

  /* Singleton */
  static Core *singleton;
  Core(void) {
    SegmentManager *sm = SegmentManager::get_instance();
    this->mState = kCMStateIdle;
  }

  CMState mState;
  std::mutex mStateLock;

  /*
   * Adapter List
   *  - N Data Adapters (+ access lock)
   *  - 1 Control Adapter (+ access lock)
   */
  std::vector<ServerAdapter *> mDataAdapters;
  ServerAdapter *mControlAdapter = NULL;
  std::mutex mDataAdaptersLock;
  std::mutex mControlAdapterLock;

  /* Control Message Listeners */
  std::vector<ControlMessageListener *> mControlMessageListeners;

  /* Statistics */
  Counter mSendRequestSize;
  ArrivalTimeCounter mSendArrivalTime;
};

} /* namespace sc */
#endif /* INC_COMMUNICATOR_H_ */
