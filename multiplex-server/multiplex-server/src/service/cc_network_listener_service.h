//
// cc_network_listener_service.h
//

#ifndef _CC_NETWORK_LISTENER_SERVICE_H
#define _CC_NETWORK_LISTENER_SERVICE_H

#include <thread>
#include <memory>

#include <netinet/in.h>

#include "cc_config.h"
#include "cc_macros.h"
#include "cc_log.h"
#include "cc_attributes.h"
#include "cc_network_address.h"
#include "cc_thread_service.h"
#include "cc_networks.h"

namespace cc {

class c_network_listener_service : protected c_attributes_exit,
protected c_attributes_alive,
protected c_attributes_id,
public c_thread_service {

public:
	virtual ~c_network_listener_service() {
    if (!c_networks::close_fd(_listener_fd)) {
      LOG_ERROR("error closing network socket ", _listener_fd);
    }
	}

  virtual bool is_attribute_exit_instance() const override final {
    return c_attributes_exit::is_attribute_exit_instance();
  }

  virtual bool is_attribute_alive_instance() const override final  {
    return c_attributes_alive::is_attribute_alive_instance();
  }

  virtual void set_instance_exit_flag() override final {
    c_attributes_exit::set_instance_exit_flag();
  }
  
public:


protected:
  c_network_listener_service();

private:
	const int32_t _port;
  const std::unique_ptr<c_network_address> _network_address;
  int32_t _listener_fd;
  
protected:
  inline int32_t port() const {
    return _port;
  };
  
  inline int32_t listener_fd() const {
    return _listener_fd;
  };

private:
  int32_t get_listener_fd();

private:
  c_network_listener_service& operator=(const c_network_listener_service&) = delete;
  c_network_listener_service(const c_network_listener_service&) = delete;
};

}

#endif /* _CC_NETWORK_LISTENER_SERVICE_H */

