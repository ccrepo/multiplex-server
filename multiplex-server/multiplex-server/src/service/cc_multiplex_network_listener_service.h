//
// cc_multiplex_network_listener_service.h
//

#ifndef _CC_MULTIPLEX_NETWORK_LISTENER_SERVICE_H
#define _CC_MULTIPLEX_NETWORK_LISTENER_SERVICE_H

#include "cc_network_listener_service.h"

#include "cc_log.h"
#include "cc_network_connections.h"
#include "cc_network_address.h"

namespace cc {

class c_multiplex_network_listener_service final : public c_network_listener_service {

public:
  c_multiplex_network_listener_service() : _network_connections(new c_network_connections()) {}

	~c_multiplex_network_listener_service() {
    _network_connections.reset();
	}

public:
	virtual bool start_thread_service() override final;
  
  virtual bool stop_thread_service() override final;

private:
  bool service();
  virtual bool service_loop() override;
  
  bool connect_listen();
  int32_t connect_select(int32_t maximum_fd);
  int32_t connect_accept(c_network_address& remote_network_address);

  bool is_new_client();

  bool update_network_connections();
  bool close_network_connections();
  
private:
  std::shared_ptr<c_network_connections> _network_connections;
  
private:
	c_multiplex_network_listener_service(const c_multiplex_network_listener_service&) = delete;
	c_multiplex_network_listener_service& operator=(const c_multiplex_network_listener_service&) = delete;
};

}

#endif /* _CC_MULTIPLEX_NETWORK_LISTENER_SERVICE_H */

 
