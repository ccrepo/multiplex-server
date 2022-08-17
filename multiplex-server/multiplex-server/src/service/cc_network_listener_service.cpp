//
// cc_network_listener_service.cpp
//

#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cc_network_listener_service.h"

#include "cc_autoset.h"
#include "cc_log.h"
#include "cc_system.h"
#include "cc_sleep.h"

namespace cc {

c_network_listener_service::c_network_listener_service() : _port(c_config::singleton()->network_connection_port()),
_network_address(new c_network_address(_port)),
_listener_fd(get_listener_fd())  {
  
  if (_listener_fd < 0) {
    LOG_FATAL("unable to listen on port/interface");
  }
}

int32_t c_network_listener_service::get_listener_fd() {
  
  int32_t fd(-1);
  int32_t code(0);
  int32_t flag(1);
  
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		LOG_ERROR("socket create failed");
		return -1;
	}

  LOG_DEBUG("socket ok fd ", fd);

  // allow more than one bind to port.
  //if ((code = setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)))) {
  //	LOG_ERROR("setsockopt failed ", code);
  //	return -1;
  //}
 
	if ((code = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))) {
		LOG_ERROR("setsockopt failed code ", code);
		return -1;
	}

	if (bind(fd, _network_address->address_as_sockaddr(), _network_address->address_size()) < 0) {
		LOG_ERROR("bind port ", port(), " ", _network_address->get_ntoa(), " failed");
		return -1;
	}

  LOG_INFO("bound ", port(), " fd ", fd, " ", _network_address->get_ntoa(), " ", _network_address->get_ntohs(), " ok");

	return fd;
}

}
