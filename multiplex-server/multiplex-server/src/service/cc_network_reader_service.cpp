//
// cc_network_reader_service.cpp
//

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <unistd.h>
#include <sys/socket.h>

#include "cc_network_reader_service.h"

#include "cc_json.h"
#include "cc_config.h"
#include "cc_sleep.h"
#include "cc_autocall.h"
#include "cc_server_pipeline.h"
#include "cc_server_pipeline_element.h"
#include "cc_network_timeval.h"
#include "cc_network_listener_service.h"

namespace cc {

c_network_reader_service::c_network_reader_service() : c_attributes_exit(),
c_attributes_alive(),
_network_buffer_size(c_config::singleton()->network_connection_buffer_size() + 1),
_network_buffer(new char[_network_buffer_size]),
_result_buffer() {
}

bool c_network_reader_service::service_loop() {
  
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

bool c_network_reader_service::publish_to_datamodel_pipe(NETWORK_READER_STATUS status,
                                                         const std::shared_ptr<c_server_pipeline_element>& server_task,
                                                         OUT c_autocall<void()>& autocall_fail) {
  
  if (!server_task->set_request_string(this->_result_buffer.str())) {
    LOG_ERROR("set request string failed");
    return false;
  }
  
  if (!c_server_pipeline::singleton()->publish_to_datamodel_pipe(attributes_id(), server_task)) {
    LOG_ERROR("publish to datamodel not ok");
    return false;
  }
  
  autocall_fail.callback_cancelled();

  return true;
}

bool c_network_reader_service::publish_to_pipe_not_ok(NETWORK_READER_STATUS status,
                                                      const std::shared_ptr<c_server_pipeline_element>& server_task) {

  // TODO - treatiing them all the same for now. closeing connection.
  
  server_task->mark_as_close_connection_required();
  
  if (!c_server_pipeline::singleton()->publish_to_completed_pipe(attributes_id(), server_task)) {
    LOG_ERROR("publish to completed not ok");
    return false;
  }
  
  return true;
}

bool c_network_reader_service::publish_to_pipe(NETWORK_READER_STATUS status,
                                               const std::shared_ptr<c_server_pipeline_element>& server_task,
                                               OUT c_autocall<void()>& autocall_fail) {

  switch (status) {
    case READER_STATUS_OK:
      return publish_to_datamodel_pipe(status, server_task, autocall_fail);
      break;

    case READER_STATUS_NONE:
    case READER_STATUS_MESSAGE_TOO_LONG:
    case READER_STATUS_MESSAGE_BIGGER_THAN_HEADER_VALUE:
    case READER_STATUS_HEADER_INVALID:
    case READER_STATUS_HEADER_MISSING:
    case READER_STATUS_NETWORK_ERROR:
    case READER_STATUS_NETWORK_TOO_MANY_READS:
    case READER_STATUS_SYSTEM_ERROR:
    case READER_STATUS_SYSTEM_EXIT:
    case READER_STATUS_CONNECTION_CLOSED:
      return publish_to_pipe_not_ok(status, server_task);
      break;
  }
  
  assert(!"should not get here!!");
  return false;
}

bool c_network_reader_service::service() {

  std::shared_ptr<c_server_pipeline_element> server_task;

  while(!is_attribute_exit_any()) {
    
    LOG_TRACE("service looping");
    
    if (server_task.get()) {
      server_task.reset();
    }
    
    if (c_server_pipeline::singleton()->get_next_request(attributes_id(), server_task) &&
        server_task.get()) {
      
      LOG_DEBUG("got task ", server_task.get());

      c_autocall<void()> autocall_unlock(server_task->set_unlock_lambda());
      c_autocall<void()> autocall_fail(server_task->set_is_failed_request_lambda());
      
      if(!publish_to_pipe(service_read(server_task), server_task, autocall_fail)) {
        LOG_WARN("did not publish datamodel ", server_task.get());
      } else {
        LOG_INFO("publish datamodel ok ", server_task.get());
      }
    } else {
      LOG_TRACE("no connect locked so looping");
    } 
    
    if (cc::c_config::singleton()->internal_test_speed_slow()) {
      c_sleep::sleep_for_random_in_range(c_duration::_5000_ms, c_duration::_10000_ms);
    } else {
      c_sleep::sleep_for_random_in_range(c_duration::_25_ms, c_duration::_50_ms);
    }
  }
  
  return true;
}
 
c_network_reader_service::NETWORK_READER_STATUS
c_network_reader_service::service_read(std::shared_ptr<c_server_pipeline_element>& server_task) {
  
  LOG_INFO("service read on port ", c_config::singleton()->network_connection_port(), " client fd ", server_task->client_fd(), " ok ", server_task.get());

  static const int32_t configured_retry_limit(c_config::singleton()->network_connection_read_retry());
  static const int32_t configured_protocol_header_size_length(c_config::singleton()->protocol_header_size_length());

  memset(_network_buffer.get(), 0, sizeof(char) * (_network_buffer_size));
  _result_buffer.str("");
  
  int32_t code(0);
  int32_t error_loop_count(0);
  int64_t header_message_length(0);
  
  std::ostringstream result_buffer;
  
  if ((code = setsockopt(server_task->client_fd(), SOL_SOCKET, SO_RCVTIMEO,
                         c_network_timeval(c_config::singleton()->network_connection_read_timeout_ms()).timeval(),
                         sizeof(timeval)))) {
    LOG_ERROR("setsockopt failed code ", code);
    return READER_STATUS_SYSTEM_ERROR;
  }

  while (true) {
    
    if (error_loop_count >= configured_retry_limit) {
      break;
    }

    if (is_attribute_exit_any()) {
      LOG_TRACE("exit flag set ", server_task.get());
      return READER_STATUS_SYSTEM_EXIT;
    }

    int64_t count(recv(server_task->client_fd(), _network_buffer.get(), _network_buffer_size - 1, 0));
    
    if (count > 0) {
      _network_buffer.get()[count] = '\0';
      result_buffer << _network_buffer;
    } else if (count < 0) {
      error_loop_count++;
    } else {
      LOG_DEBUG(server_task->client_fd(), " is not connected - zero bytes read");
      return READER_STATUS_CONNECTION_CLOSED;
    }
    
    LOG_DEBUG("recv ", count, " for total ", result_buffer.tellp(), " bytes ", server_task.get());
    
    if (header_message_length == 0 &&
        result_buffer.tellp() > configured_protocol_header_size_length) {
      
      header_message_length = c_strings::extract_leading_number_length_n(result_buffer.str(), configured_protocol_header_size_length);
      
      LOG_DEBUG("extracted in header message length ", header_message_length, " ", server_task.get());

      if (header_message_length <= 0) {
        LOG_ERROR("recv not ok extracted in header message length not ok ", header_message_length, " ", server_task.get());
        return READER_STATUS_HEADER_INVALID;
      }
    }
    
    if (result_buffer.tellp() > c_config::singleton()->network_connection_clients_max_message_size()) {
      LOG_ERROR("recv not ok message too long ", result_buffer.tellp(), " ", server_task.get());
      return READER_STATUS_MESSAGE_TOO_LONG;
    }
    
    if (header_message_length > 0 && result_buffer.tellp() > header_message_length) {
      LOG_ERROR("recv msg longer than config length ", header_message_length, " ", result_buffer.tellp(), " ", server_task.get());
      return READER_STATUS_MESSAGE_BIGGER_THAN_HEADER_VALUE;
    }
    
    if (header_message_length <= 0 && result_buffer.tellp() > configured_protocol_header_size_length) {
      LOG_ERROR("recv not ok message header missing ", server_task.get());
      return READER_STATUS_HEADER_MISSING;
    }
  }
  
  if (header_message_length > 0 && result_buffer.tellp() == header_message_length) {
    _result_buffer << result_buffer.str().substr(configured_protocol_header_size_length);
    LOG_INFO("recv ok ", _result_buffer.str(), " ", server_task.get());
    return READER_STATUS_OK;
  }
  
  if (error_loop_count >= configured_retry_limit) {
    LOG_ERROR("recv not ok try limit reached with incorrect acount of data ", result_buffer.tellp(), " ", server_task.get());
    return READER_STATUS_NETWORK_TOO_MANY_READS;
  }

  LOG_ERROR("recv not ok ", server_task.get());
  return READER_STATUS_SYSTEM_ERROR;
}

bool c_network_reader_service::stop_thread_service() {

  this->set_instance_exit_flag();
  
  int32_t limit(static_cast<int32_t>(c_config::singleton()->internal_wait_service_exit() / 25) + 1);
  
  while (true) {
    
    if (!is_instance_alive_relaxed() &&
        !is_instance_alive_seq_cst()) {
      return true;
    }
    
    if (--limit < 0) {
      LOG_WARN("network reader still running on exit");
      return false;
    }
    
    c_sleep::sleep_25_ms(); // TODO cond var
  }
  
  return true;
}

}

