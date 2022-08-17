//
// cc_network_connection.h
//

#ifndef _CC_NETWORK_CONNECTION_H
#define _CC_NETWORK_CONNECTION_H

#include <thread>
#include <memory>

#include "cc_config.h"
#include "cc_macros.h"
#include "cc_log.h"

namespace cc {

class c_network_connection final {

public:
  c_network_connection(int32_t client_fd) : _connection_id(++_connection_id_counter), _client_fd(client_fd), _locked(false) {}
  
  ~c_network_connection() {}
  
  inline int32_t client_fd() const {
    return _client_fd;
  }
  
  inline bool locked() const {
    return _locked.load(std::memory_order_seq_cst);
  }

  inline void lock() {
    _locked.store(true, std::memory_order_seq_cst);
  }

  inline void unlock() {
    _locked.store(false, std::memory_order_seq_cst);
  }

private:
  const long long _connection_id;
  const int32_t _client_fd;
  std::atomic<bool> _locked;
  
private:
  inline static std::atomic<long long> _connection_id_counter { 0 };

private:
  c_network_connection& operator=(const c_network_connection&) = delete;
  c_network_connection(const c_network_connection&) = delete;

};

}

#endif /* _CC_NETWORK_CONNECTION_H */

