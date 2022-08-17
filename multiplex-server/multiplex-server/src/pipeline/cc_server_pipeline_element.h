//
// cc_server_pipeline_element.h
//

#ifndef _CC_SERVER_PIPELINE_ELEMENT_H
#define _CC_SERVER_PIPELINE_ELEMENT_H

#include <memory>
#include <atomic>
#include <string>

#include "cc_config.h"
#include "cc_time.h"
#include "cc_log.h"

namespace cc {

class c_server_pipeline_element final {
  
public:
  c_server_pipeline_element(int32_t client_fd);
  
  ~c_server_pipeline_element() {}
  
  bool lock_with_owner_lockable(uint64_t object_id);
  
  void unlock();

  inline int32_t client_fd() const {
    return _client_fd;
  }
  
  inline uint64_t locked_by_id() const {
    return _locked_by_id;
  }
    
  //
  
  inline bool is_datamodel_failed() const {
    return _is_datamodel_failed.load(std::memory_order_relaxed) &&
    _is_datamodel_failed.load(std::memory_order_seq_cst);
  }
  
  inline bool is_request_failed() const {
    return _is_request_failed.load(std::memory_order_relaxed) &&
    _is_request_failed.load(std::memory_order_seq_cst);
  }
  
  inline bool is_response_failed() const {
    return _is_response_failed.load(std::memory_order_relaxed) &&
    _is_response_failed.load(std::memory_order_seq_cst);
  }
  
  inline bool is_completed_failed() const {
    return _is_completed_failed.load(std::memory_order_relaxed) &&
    _is_completed_failed.load(std::memory_order_seq_cst);
  }
  
  //
  
  inline bool is_datamodel_completed() const {
    return _is_datamodel_completed.load(std::memory_order_relaxed) &&
    _is_datamodel_completed.load(std::memory_order_seq_cst);
  }
  
  inline bool is_request_completed() const {
    return _is_request_completed.load(std::memory_order_relaxed) &&
    _is_request_completed.load(std::memory_order_seq_cst);
  }
  
  inline bool is_response_completed() const {
    return _is_response_completed.load(std::memory_order_relaxed) &&
    _is_response_completed.load(std::memory_order_seq_cst);
  }

  inline bool is_completed_completed() const {
    return _is_completed_completed.load(std::memory_order_relaxed) &&
    _is_completed_completed.load(std::memory_order_seq_cst);
  }

  inline bool is_close_connection_required() const {
    return _is_close_connection_required.load(std::memory_order_relaxed) &&
    _is_close_connection_required.load(std::memory_order_seq_cst);
  }

  //

  inline const std::ostringstream& response() const {
    return _response;
  }
  
  inline const std::ostringstream& request() const {
    return _request;
  }
  
  //
  
  inline void mark_as_datamodel_failed() {
    _is_datamodel_failed.store(true);
  }
  
  inline void mark_as_request_failed() {
    _is_request_failed.store(true);
  };
  
  inline void mark_as_response_failed() {
    _is_response_failed.store(true);
  }
  
  inline void mark_as_completed_failed() {
    _is_completed_failed.store(true);
  }
  
  //
  
  inline void mark_as_datamodel_completed() {
    _is_datamodel_completed.store(true);
  }
  
  inline void mark_as_request_completed() {
    _is_request_completed.store(true);
  };
  
  inline void mark_as_response_completed() {
    _is_response_completed.store(true);
  }

  inline void mark_as_completed_completed() {
    _is_completed_completed.store(true);
  }

  inline void mark_as_close_connection_required() {
    _is_close_connection_required.store(true);
  }
  
  //
  
  inline bool set_request_string(const std::string& s) {
    if (_request.tellp() > 0) {
      return false;
    }
    _request << s;
    return true;
  }
  
  inline bool set_response_string(const std::string& s) {
    if (_response.tellp() > 0) {
      return false;
    }
    _response << s;
    return true;
  }
  
  //
  
  std::function<void ()> set_unlock_lambda() {
    return [this]() {
      this->unlock();
    };
  }

  std::function<void ()> set_is_failed_completed_lambda() {
    return [this]() {
      LOG_TRACE("set completed failed ", this);
      _is_completed_failed.store(true);
    };
  }

  std::function<void ()> set_is_failed_datamodel_lambda() {
    return [this]() {
      LOG_TRACE("set datamodel failed ", this);
      _is_datamodel_failed.store(true);
    };
  }

  std::function<void ()> set_is_failed_request_lambda() {
    return [this]() {
      LOG_TRACE("set request failed ", this);
      _is_request_failed.store(true);
    };
  }

  std::function<void ()> set_is_failed_response_lambda() {
    return [this]() {
      LOG_TRACE("set response failed ", this);
      _is_response_failed.store(true);
    };
  }

private:
  const int32_t         _client_fd;
  
  std::atomic<bool>     _is_locked;
  std::atomic<uint64_t> _locked_by_id;
  
  std::atomic<bool>     _is_close_connection_required;

  // TODO encode these 8 as binary bits
  
  std::atomic<bool>     _is_completed_failed;
  std::atomic<bool>     _is_datamodel_failed;
  std::atomic<bool>     _is_request_failed;
  std::atomic<bool>     _is_response_failed;

  std::atomic<bool>     _is_completed_completed;
  std::atomic<bool>     _is_datamodel_completed;
  std::atomic<bool>     _is_request_completed;
  std::atomic<bool>     _is_response_completed;
  
  std::ostringstream    _request;
  std::ostringstream    _response;
  
private:
  c_server_pipeline_element(const c_server_pipeline_element&) = delete;
  c_server_pipeline_element& operator=(const c_server_pipeline_element&) = delete;
  
};

}

#endif /* _CC_SERVER_PIPELINE_ELEMENT_H */
