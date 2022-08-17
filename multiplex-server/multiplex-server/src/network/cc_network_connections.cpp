//
// cc_network_connections.cpp
//

#include "cc_network_connections.h"
#include "cc_networks.h"

namespace cc {

int32_t c_network_connections::get_max_fd(int32_t fd) {
  int32_t max_sd = fd;
  auto& network_sockets(_network_sockets);
  
  std::for_each(_network_connections.get()->begin(),
                _network_connections.get()->end(),
                [&max_sd, &network_sockets](std::shared_ptr<c_network_connection>& network_connection) {
    
    if (network_connection.get() &&
        network_connection.get()->client_fd() > 0) {
      FD_SET(network_connection.get()->client_fd(), &network_sockets);
    }
    
    if (network_connection.get() &&
        network_connection.get()->client_fd() > max_sd) {
      max_sd = network_connection.get()->client_fd();
    }
  });
  
  return max_sd;
}

bool c_network_connections::add_connection(int32_t new_fd) {
  auto iter { std::find_if(_network_connections.get()->begin(),
                           _network_connections.get()->end(), [](std::shared_ptr<c_network_connection>& network_connection) { return !network_connection.get(); } ) };
  
  if (iter == _network_connections.get()->end()) {
    return false;
  }
  
  auto& network_connection(*iter);
  
  if (!network_connection.get()) {
    network_connection.reset(new c_network_connection(new_fd));
  }
  
  return true;
}

const std::vector<int32_t>& c_network_connections::get_active_connections() {
  
  auto& network_sockets(_network_sockets);
  auto& active_connections(_active_connections);
  
  std::for_each(_network_connections.get()->begin(),
                _network_connections.get()->end(),
                [&network_sockets, &active_connections](std::shared_ptr<c_network_connection>& network_connection) {
    
    if (network_connection.get() &&
        network_connection.get()->client_fd() > 0) {
      
      if (FD_ISSET(network_connection.get()->client_fd(), &network_sockets)) {
        if (!network_connection.get()->locked()) {
          LOG_DEBUG("client ", network_connection.get()->client_fd(), " setting lock ", network_connection.get());
          network_connection.get()->lock();
          active_connections.push_back(network_connection.get()->client_fd());
        } else {
          LOG_DEBUG("client ", network_connection.get()->client_fd(), " is locked and ignored ", network_connection.get());
        }
      }
    }
  });
  
  return _active_connections;
}

bool c_network_connections::remove_connection(int32_t fd) {
  auto iter { std::find_if(_network_connections.get()->begin(),
                           _network_connections.get()->end(),
                           [fd](std::shared_ptr<c_network_connection>& network_connection) {
    if (network_connection.get() &&
        network_connection.get()->client_fd() == fd &&
        network_connection.get()->locked()) {
      return true;
    }
    return false;
  })};
  
  if (iter != _network_connections.get()->end()) {
    iter->reset();
    return true;
  }
  return false;
}

bool c_network_connections::unlock_connection(int32_t fd) {
  auto iter { std::find_if(_network_connections.get()->begin(),
                           _network_connections.get()->end(),
                           [fd](std::shared_ptr<c_network_connection>& network_connection) {
    if (network_connection.get() &&
        network_connection.get()->client_fd() == fd &&
        network_connection.get()->locked()) {
      return true;
    }
    return false;
  })};
  
  if (iter != _network_connections.get()->end()) {
    iter->get()->unlock();
    return true;
  }
  return false;
}

bool c_network_connections::close_connections() {
  bool result(true);
  auto& network_sockets(_network_sockets);
  
  std::for_each(_network_connections.get()->begin(),
                _network_connections.get()->end(),
                [&result, &network_sockets](std::shared_ptr<c_network_connection>& network_connection) {
    
    if (network_connection.get() &&
        network_connection.get()->client_fd() > 0) {
      
      LOG_INFO("close fd ", network_connection.get()->client_fd());
            
      if (!c_networks::close_fd(network_connection.get()->client_fd())) {
        LOG_ERROR("error closing network socket ", network_connection.get()->client_fd());
      }
      
      network_connection.reset();
    }
  });
  
  return result;
}

void c_network_connections::dump() {
  int idx(0);
  std::ostringstream os;
  os << std::endl
  << "connections dump: " << std::endl;
  
  std::for_each (_network_connections.get()->begin(),
                 _network_connections.get()->end(),
                 [&idx, &os](const auto& network_connection){
    if (network_connection.get()) {
      
      os << "connection: " << ++idx << " " << network_connection.get()->client_fd() << "\t"
      << " locked " << (network_connection.get()->locked() ? "yes" : "no") << std::endl;
      
    } else {
      os << "connection: " << ++idx << " \t" << " locked no" << std::endl;
    }
  });
  
  os << "-";
  LOG_INFO(os.str());
}

}

