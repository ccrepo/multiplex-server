//
// cc_network_address.h
//

#ifndef _CC_NETWORK_ADDRESS_H
#define _CC_NETWORK_ADDRESS_H

#include <algorithm>
#include <memory>
#include <cstdio>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "cc_config.h"
#include "cc_networks.h"
#include "cc_log.h"

namespace cc {

class c_network_address final {

public:
  c_network_address(int32_t port) : _address(new sockaddr_in()),
  _address_size(static_cast<socklen_t>(sizeof(sockaddr_in))) {
    _address->sin_family = AF_INET;

    const std::string& ip(c_config::singleton()->network_connection_bind_ip());
    
    if (ip.empty()) {
      _address->sin_addr.s_addr = INADDR_ANY;
    } else if (!cc::c_networks::is_ipv4_address_format(ip)) {
      LOG_FATAL("invalid ip address config ", ip)
      _address->sin_addr.s_addr = INADDR_ANY;
    } else {
      _address->sin_addr.s_addr = inet_addr(ip.c_str());
    }
    
    _address->sin_port = htons(port);
  }

  c_network_address() : c_network_address(0) {
  }
  
  ~c_network_address() {
	}

  inline const std::unique_ptr<sockaddr_in>& address() const {
    return _address;
  }

  inline socklen_t* address_size_as_ptr() {
    return &_address_size;
  }

  inline socklen_t address_size() const {
    return _address_size;
  }

  inline sockaddr* address_as_sockaddr() {
    return reinterpret_cast<sockaddr*>(_address.get());
  }

  inline const char* get_ntoa() const {
    return inet_ntoa(_address->sin_addr);
  }
  
  inline uint32_t get_ntohs() const {
    return ntohs(_address->sin_port);
  }
  
private:
  std::unique_ptr<sockaddr_in> _address;
  socklen_t _address_size;
  
private:
  c_network_address(const c_network_address&) = delete;
  c_network_address& operator=(const c_network_address&) = delete;
};

} 

#endif /* _CC_NETWORK_ADDRESS_H */

