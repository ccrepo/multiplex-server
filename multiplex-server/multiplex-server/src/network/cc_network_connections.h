//
// cc_network_connections.h
//

#ifndef _CC_NETWORK_CONNECTIONS_H
#define _CC_NETWORK_CONNECTIONS_H

#include <thread>
#include <memory>
#include <array>

#include <unistd.h>
#include <sys/socket.h>

#include "cc_config.h"
#include "cc_macros.h"
#include "cc_log.h"
#include "cc_attributes.h"
#include "cc_network_connection.h"

namespace cc {

class c_multiplex_network_listener_service;

class c_network_connections final {

public:
  ~c_network_connections() {
    close_connections();
	}
  
  inline bool lock(uint64_t owner_object_id) {
    static uint64_t owner_object_id_zero(0);
    return std::atomic_compare_exchange_weak(&_locked_by_id, &owner_object_id_zero, owner_object_id);
  }

  inline void unlock() {
    _locked_by_id.store(0);
  }

  std::function<void ()> get_unlock_lambda() {
    return [this]() {
      this->unlock();
    };
  }

  inline void init(int32_t listener_fd) {
    FD_ZERO(&_network_sockets);
    FD_SET(listener_fd, &_network_sockets);
  }
  
  inline fd_set* network_sockets() {
    return &_network_sockets;
  }
  
  inline size_t get_pending_connections_count() const {
    return _active_connections.size();
  }
  
  inline void clear_pending_connections() {
    _active_connections.clear();
  }
  
  int32_t get_max_fd(int32_t fd);
  bool add_connection(int32_t new_fd);
  bool close_connections();
  const std::vector<int32_t>& get_active_connections();
  bool remove_connection(int32_t fd);
  bool unlock_connection(int32_t fd);
  
private:
  c_network_connections() : _locked_by_id(0),
  _maximum_clients(c_config::singleton()->network_connection_clients_max_concurrent()),
  _network_connections(new std::vector<std::shared_ptr<c_network_connection>>(_maximum_clients)),
  _network_sockets() {
  }
  
private:
  friend class c_multiplex_network_listener_service;
  
private:
  std::atomic<uint64_t> _locked_by_id;
  const int32_t _maximum_clients;
  const std::unique_ptr<std::vector<std::shared_ptr<c_network_connection>>> _network_connections;
  fd_set _network_sockets;
  std::vector<int32_t> _active_connections;
  void dump();

private:
  c_network_connections& operator=(const c_network_connections&) = delete;
  c_network_connections(const c_network_connections&) = delete;
};

}

#endif /* _CC_NETWORK_CONNECTIONS_H */

