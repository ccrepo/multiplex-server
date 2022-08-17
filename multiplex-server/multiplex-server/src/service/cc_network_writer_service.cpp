//
// cc_network_writer_service.cpp
//

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <unistd.h>
#include <sys/socket.h>

#include "cc_network_writer_service.h"

#include "cc_math.h"
#include "cc_json.h"
#include "cc_config.h"
#include "cc_sleep.h"
#include "cc_autocall.h"
#include "cc_server_pipeline.h"
#include "cc_server_pipeline_element.h"
#include "cc_network_timeval.h"
#include "cc_networks.h"

namespace cc {

c_network_writer_service::c_network_writer_service() : c_attributes_exit(),
c_attributes_alive(),
_network_buffer_size(c_config::singleton()->network_connection_buffer_size() + 1) {
}

bool c_network_writer_service::service_loop() {
  
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

bool c_network_writer_service::publish_to_completed_pipe(NETWORK_WRITER_STATUS status,
                                                         const std::shared_ptr<c_server_pipeline_element>& server_task,
                                                         OUT c_autocall<void()>& autocall_fail) {
  
  if (!c_server_pipeline::singleton()->publish_to_completed_pipe(attributes_id(), server_task)) {
    LOG_ERROR("publish to completed not ok");
    return false;
  }
  
  autocall_fail.callback_cancelled();

  return true;
}

bool c_network_writer_service::publish_to_pipe_not_ok(NETWORK_WRITER_STATUS status,
                                                      const std::shared_ptr<c_server_pipeline_element>& server_task) {
  
  server_task->mark_as_close_connection_required();
  
  if (!c_server_pipeline::singleton()->publish_to_completed_pipe(attributes_id(), server_task)) {
    LOG_ERROR("publish to completed not ok");
    return false;
  }
  
  return true;
}

bool c_network_writer_service::publish_to_pipe(NETWORK_WRITER_STATUS status,
                                               const std::shared_ptr<c_server_pipeline_element>& server_task,
                                               OUT c_autocall<void()>& autocall_fail) {
  
  switch (status) {

    case WRITER_STATUS_OK:
      return publish_to_completed_pipe(status, server_task, autocall_fail);
      break;

    case WRITER_STATUS_NONE:
    case WRITER_STATUS_NETWORK_ERROR:
    case WRITER_STATUS_SYSTEM_ERROR:
    case WRITER_STATUS_SYSTEM_EXIT:
      return publish_to_pipe_not_ok(status, server_task);
      break;
  }
  
  assert(!"should not get here!!");
  return false;
}

bool c_network_writer_service::service() {
  
  std::shared_ptr<c_server_pipeline_element> server_task;
  
  while(!is_attribute_exit_any()) {

    LOG_TRACE("service looping");
    
    if (server_task.get()) {
      server_task.reset();
    }
    
    if (c_server_pipeline::singleton()->get_next_response(attributes_id(), server_task) &&
        server_task.get()) {

      LOG_DEBUG("got task ", server_task.get());
      
      c_autocall<void()> autocall_unlock(server_task->set_unlock_lambda());
      c_autocall<void()> autocall_fail(server_task->set_is_failed_response_lambda());
      
      if(!publish_to_pipe(service_write(server_task), server_task, autocall_fail)) {
        LOG_WARN("could not publish completed ", server_task.get());
      } else {
        LOG_INFO("publish to response ok ", server_task.get());
      }
      
    } else {
      LOG_TRACE("no datamodel locked so looping");
    }
    
    if (cc::c_config::singleton()->internal_test_speed_slow()) {
      c_sleep::sleep_for_random_in_range(c_duration::_5000_ms, c_duration::_10000_ms);
    } else {
      c_sleep::sleep_for_random_in_range(c_duration::_25_ms, c_duration::_50_ms);
    }
  }
  
  return true;
}

bool c_network_writer_service::stop_thread_service() {

  this->set_instance_exit_flag();
  
  int32_t limit(static_cast<int32_t>(c_config::singleton()->internal_wait_service_exit() / 25) + 1);
  
  while (true) {
    
    if (!is_instance_alive_relaxed() &&
        !is_instance_alive_seq_cst()) {
      return true;
    }
    
    if (--limit < 0) {
      LOG_WARN("network writer still running on exit");
      return false;
    }
    
    c_sleep::sleep_25_ms(); // TODO cond var
  }
  
  return true;
}

c_network_writer_service::NETWORK_WRITER_STATUS
c_network_writer_service::service_write(std::shared_ptr<c_server_pipeline_element>& server_task) {

  LOG_INFO("service send on port ", c_config::singleton()->network_connection_port(),
           " client fd ", server_task->client_fd(), " ok ", server_task.get());

  static const int32_t configured_retry_limit(c_config::singleton()->network_connection_write_retry());
  
  int32_t code(0);
  int32_t error_loop_count(0);
  int64_t sent_message_length(0);
  
  if ((code = setsockopt(server_task->client_fd(), SOL_SOCKET, SO_SNDTIMEO,
                         c_network_timeval(c_config::singleton()->network_connection_write_timeout_ms()).timeval(),
                         sizeof(timeval)))) {
    
    LOG_ERROR("setsockopt failed code ", code);
    return WRITER_STATUS_SYSTEM_ERROR;
  }
  
  while (true) {
    
    if (error_loop_count >= configured_retry_limit) {
      return WRITER_STATUS_NETWORK_ERROR;
    }

    if (is_attribute_exit_any()) {
      LOG_TRACE("exit flag set ", server_task.get());
      return WRITER_STATUS_SYSTEM_EXIT;
    }
    
    const std::string response(server_task->response().str());

    // TODO not sent data "in buffer"
    size_t count(send(server_task->client_fd(), &response[sent_message_length],
                      cc::c_math::min<int64_t>(_network_buffer_size - 1,
                                               static_cast<int64_t>(response.length()) - sent_message_length), 0));

    if (count <= 0) {
      error_loop_count++;
      continue;
    }
    
    if ((sent_message_length += count) >= response.length()) {
      LOG_INFO("sent data on port ", c_config::singleton()->network_connection_port(),
               " client fd ", server_task->client_fd(), " ok ", server_task.get(), server_task->response());
      
      return WRITER_STATUS_OK;
    }
  }
   
  return WRITER_STATUS_NONE;
}

}



