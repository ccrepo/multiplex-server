//
// cc_multiplex_network_listener_service.cpp
//

#include <cstdio>
#include <cstdlib>

#include <netinet/in.h>

#include "cc_multiplex_network_listener_service.h"

#include "cc_autoset.h"
#include "cc_autocall.h"
#include "cc_sleep.h"
#include "cc_network_timeval.h"
#include "cc_network_address.h"
#include "cc_server_pipeline.h"
#include "cc_networks.h"

namespace cc {

bool c_multiplex_network_listener_service::start_thread_service() {
  return service_loop();
}

bool c_multiplex_network_listener_service::stop_thread_service() {

  this->set_instance_exit_flag();
  
  int32_t limit(static_cast<int32_t>(c_config::singleton()->internal_wait_service_exit() / 25) + 1);
  
  while (true) {
    
    if (!is_instance_alive_relaxed() &&
        !is_instance_alive_seq_cst()) {
      return true;
    }
    
    if (--limit < 0) {
      LOG_WARN("multiplex listener still running on exit");
      return false;
    }
    
    c_sleep::sleep_25_ms(); // TODO cond var
  }

  return true;
}

bool c_multiplex_network_listener_service::service_loop() {
  
  c_autocall<void()> autocall(get_set_instance_not_alive_lambda());

  while (!is_attribute_exit_any()) {
    LOG_TRACE("service loop");
    
    if (!service()) {
      LOG_ERROR("service failed");
      return false;
    }
    
    c_sleep::sleep_for_random_in_range(c_duration::_25_ms, c_duration::_50_ms); // TODO cond var
  }

  return true;
}

bool c_multiplex_network_listener_service::connect_listen() {

  std::ostringstream os;
  
  if (cc::c_logger_implT<cc::log::c_error>::is_level_on() ||
      cc::c_logger_implT<cc::log::c_warn>::is_level_on() ||
      cc::c_logger_implT<cc::log::c_debug>::is_level_on()) {
    
    os << port()
    << " "
    << listener_fd()
    << " "
    << c_networks::get_errno_description(errno);
  }
  
  int32_t code(listen(listener_fd(), c_config::singleton()->network_connection_clients_max_pending()));
  
  if (code < 0) {
    LOG_ERROR(os, " failed ", code);
    return false;
  }
  
  return true;
}

int32_t c_multiplex_network_listener_service::connect_select(int32_t maximum_fd) {
  
  int32_t code(select(maximum_fd + 1, _network_connections->network_sockets(), NULL, NULL,
                      c_network_timeval(c_config::singleton()->network_connection_read_timeout_ms()).timeval()));
 
  std::ostringstream os;
  
  if (cc::c_logger_implT<cc::log::c_error>::is_level_on() ||
      cc::c_logger_implT<cc::log::c_warn>::is_level_on() ||
      cc::c_logger_implT<cc::log::c_debug>::is_level_on()) {
    
    os << port()
    << " "
    << listener_fd()
    << " "
    << c_networks::get_errno_description(errno);
  }
  
  if (code < 0) {
    if (errno == EINTR) {
      LOG_WARN(os, " ignore ", code);
      return 0;
    }
    
    LOG_ERROR(os, " failed ", code);
    return -1;
  }
  
  if (code > 0) {
    return code;
  }
  
  LOG_TRACE(os, " timeout ", code);
  return 0;
}

int32_t c_multiplex_network_listener_service::connect_accept(c_network_address& remote_network_address) {
  
  int32_t new_fd(accept(listener_fd(), remote_network_address.address_as_sockaddr(), remote_network_address.address_size_as_ptr()));

  std::ostringstream os;
  
  if ((new_fd < 0 && cc::c_logger_implT<cc::log::c_error>::is_level_on()) ||
      cc::c_logger_implT<cc::log::c_debug>::is_level_on()) {
    os << port()
    << " "
    << listener_fd()
    << " "
    << new_fd;
  }
  
  if (new_fd < 0) {
    LOG_ERROR(os, " accept not ok");
    return new_fd;
  }
  
  LOG_DEBUG(os, " remote ", remote_network_address.get_ntoa(), " ", remote_network_address.get_ntohs(), " ok");
  return new_fd;
}

bool c_multiplex_network_listener_service::is_new_client() {
  if (FD_ISSET(listener_fd(), _network_connections->network_sockets())) {
    return true;
  }

  return false;
}

bool c_multiplex_network_listener_service::service() {

  if (!_network_connections->lock(attributes_id())) {
    LOG_ERROR("failed lock write for owner");
    return true;
  }
 
  c_autocall<void()> autocall(_network_connections->get_unlock_lambda());

  LOG_DEBUG("listening ...");
  
  if (!connect_listen()) {
    return false;
  }
    
  while(!is_attribute_exit_any()) {
        
    LOG_TRACE("service looping");
    
    if (c_config::singleton()->diagnostics_log_dump_connections()) {
      _network_connections->dump();
    }
    
    _network_connections->init(listener_fd()); // TODO test this

    int32_t code(connect_select(_network_connections->get_max_fd(listener_fd())));

    if (code < 0) {
      LOG_DEBUG("select on fd ", listener_fd(), " failed code ", code);
      
    } else if (code > 0) {

      if (is_new_client()) {
        LOG_DEBUG("is new client true");

        c_network_address remote_network_address;

        int32_t new_fd(connect_accept(remote_network_address));
        
        if (new_fd < 0) {
          LOG_ERROR("accept client fd invalid");
          
        } else if (!_network_connections->add_connection(new_fd)) {
          LOG_WARN("max current clients reached closing new fd ", new_fd);
          
          if (!c_networks::close_fd(new_fd)) {
            LOG_ERROR("error closing network socket ", new_fd);
          }
        } else {
          LOG_DEBUG("new client fd ", new_fd, " configured");
        }
      } else {
        LOG_DEBUG("is not new client");
      }
      
    } else {
      LOG_TRACE("timeout");
    }

    const auto& active_connections(_network_connections->get_active_connections());
    
    if (!active_connections.empty()) {
      
      LOG_DEBUG("trying to publish ", active_connections.size(), " connect elements to request pipeline");
      
      if (!c_server_pipeline::singleton()->publish_connect_n(active_connections)) {
        LOG_WARN("request publish not ok");
      } else {
        LOG_DEBUG("publish to request(n) ok");
        _network_connections->clear_pending_connections();
      }
    }
    
    if (!update_network_connections()) {
      LOG_WARN("update connections failed");
    }
    
    if (cc::c_config::singleton()->internal_test_speed_slow()) {
      c_sleep::sleep_for_random_in_range(c_duration::_5000_ms, c_duration::_10000_ms);
    } else {
      c_sleep::sleep_for_random_in_range(c_duration::_25_ms, c_duration::_50_ms);
    }
  }

  return true;
}

bool c_multiplex_network_listener_service::update_network_connections() {
  
  std::shared_ptr<c_server_pipeline_element> server_task;
  
  if (is_attribute_exit_any()) {
    return true;
  }
  
  while (true) {
    
    if (c_server_pipeline::singleton()->get_next_completed(attributes_id(), server_task) &&
        server_task.get()) {
      
      LOG_TRACE("got response task ", server_task.get(), " ", server_task->client_fd());
      
      c_autocall<void()> autocall_unlock(server_task->set_unlock_lambda());

      if (server_task->is_close_connection_required()) {
        
        LOG_DEBUG(server_task->client_fd(), " is not connected so closing socket");

        if(!_network_connections->remove_connection(server_task->client_fd())) {
          LOG_ERROR(server_task->client_fd(), " could not remove from connection list");
        }
        
        if (!c_networks::close_fd(server_task->client_fd())) {
          LOG_ERROR("error closing network socket ", server_task->client_fd());
        }
      } else {
        LOG_TRACE("socket ", server_task->client_fd(), " is connected");

        if(!_network_connections->unlock_connection(server_task->client_fd())) {
          LOG_ERROR(server_task->client_fd(), " could not unlock in connection list");
        } else {
          LOG_INFO("unlocked ", server_task->client_fd(), " ", server_task.get());
        }
      }

      server_task->mark_as_completed_completed();
      
    } else {
      LOG_TRACE("no more response tasks");
      break;
    }
  }

  return true;
}

}

