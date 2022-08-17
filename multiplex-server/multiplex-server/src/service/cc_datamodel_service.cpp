//
// cc_datamodel_service.cpp
//

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>

#include <unistd.h>
#include <sys/socket.h>

#include "json.h"

#include "cc_datamodel_service.h"

#include "cc_json.h"
#include "cc_config.h"
#include "cc_sleep.h"
#include "cc_autocall.h"
#include "cc_server_pipeline.h"
#include "cc_server_pipeline_element.h"
#include "cc_messages.h"

namespace cc {

c_datamodel_service::c_datamodel_service() : c_attributes_exit(),
c_attributes_alive(),
_result_buffer() {
}

bool c_datamodel_service::service_loop() {
  
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

c_datamodel_service::DATAMODEL_STATUS
c_datamodel_service::service_datamodel(std::shared_ptr<c_server_pipeline_element>& server_task) {
  
  LOG_DEBUG("processing request ", server_task.get()->request());
  
  _result_buffer.str("");

  // NOTE: simple proof-of-concept-response ping bounce for now
  
  try {
    nlohmann::json j = nlohmann::json::parse(server_task.get()->request().str());

    std::ostringstream os;
     
    os << std::endl << std::setw(2) << j;
    
    LOG_INFO("received json ", os.str());
    
    auto node = j["body"]["ping_id"];
    
    if (node.is_number()) {
      
      int ping_id = node;
      
      j["body"]["ping_id"] = ping_id + 1;
      
      std::ostringstream os;
      
      os << std::endl << std::setw(2) << j;
      
      LOG_INFO("result json ", os.str());

      _result_buffer << j.dump();
      return c_datamodel_service::DATAMODEL_STATUS_OK;
      
    } else {

      _result_buffer << c_message_bad_request().to_string();
      return c_datamodel_service::DATAMODEL_STATUS_NOT_OK;
    }

  } catch (...) {
    
    _result_buffer << c_message_bad_request().to_string();
    return c_datamodel_service::DATAMODEL_STATUS_NOT_OK;
  }
  
  return DATAMODEL_STATUS_NONE;
}

bool c_datamodel_service::service() {

  std::shared_ptr<c_server_pipeline_element> server_task;

  while(!is_attribute_exit_any()) {
    
    LOG_TRACE("service looping");
    
    if (server_task.get()) {
      server_task.reset();
    }
    
    if (c_server_pipeline::singleton()->get_next_datamodel(attributes_id(), server_task) &&
        server_task.get()) {
      
      LOG_DEBUG("got task ", server_task.get());

      c_autocall<void()> autocall_unlock(server_task->set_unlock_lambda());
      c_autocall<void()> autocall_fail(server_task->set_is_failed_datamodel_lambda());
      
      if(!publish_to_pipe(service_datamodel(server_task), server_task, autocall_fail)) {
        LOG_WARN("could not publish response", server_task.get());
      } else {
        LOG_INFO("publish response ok ", server_task.get());
      }
      
    } else {
      LOG_TRACE("no request locked so looping");
    } 
    
    if (cc::c_config::singleton()->internal_test_speed_slow()) {
      c_sleep::sleep_for_random_in_range(c_duration::_5000_ms, c_duration::_10000_ms);
    } else {
      c_sleep::sleep_for_random_in_range(c_duration::_25_ms, c_duration::_50_ms);
    }
  }
  
  return true;
}

bool c_datamodel_service::stop_thread_service() {

  this->set_instance_exit_flag();
  
  int32_t limit(static_cast<int32_t>(c_config::singleton()->internal_wait_service_exit() / 25) + 1);
  
  while (true) {
    
    if (!is_instance_alive_relaxed() &&
        !is_instance_alive_seq_cst()) {
      return true;
    }
        
    if (--limit < 0) {
      LOG_WARN("datamodel still running on exit");
      return false;
    }
    
    c_sleep::sleep_25_ms(); // TODO cond var
  }
  
  return true;
}

bool c_datamodel_service::publish_to_pipe(DATAMODEL_STATUS status,
                                          const std::shared_ptr<c_server_pipeline_element>& server_task,
                                          OUT c_autocall<void()>& autocall_fail) {

  switch (status) {
    case DATAMODEL_STATUS_OK:
      return publish_to_response_pipe(status, server_task, autocall_fail);
      break;
      
    case DATAMODEL_STATUS_NONE:
    case DATAMODEL_STATUS_NOT_OK:
      return publish_to_pipe_not_ok(status, server_task);
      break;
  }
  
  assert(!"should not get here!!");
  return false;
}

bool c_datamodel_service::publish_to_response_pipe(DATAMODEL_STATUS status,
                                                   const std::shared_ptr<c_server_pipeline_element>& server_task,
                                                   OUT c_autocall<void()>& autocall_fail) {
  
  if (!server_task->set_response_string(this->_result_buffer.str())) {
    LOG_ERROR("set response string failed");
    return false;
  }
  
  if (!c_server_pipeline::singleton()->publish_to_response_pipe(attributes_id(), server_task)) {
    LOG_ERROR("publish to response not ok");
    return false;
  }
  
  autocall_fail.callback_cancelled();

  return true;
}

bool c_datamodel_service::publish_to_pipe_not_ok(DATAMODEL_STATUS status,
                                                 const std::shared_ptr<c_server_pipeline_element>& server_task) {
  
  server_task->mark_as_close_connection_required();
  
  if (!c_server_pipeline::singleton()->publish_to_completed_pipe(attributes_id(), server_task)) {
    LOG_ERROR("publish to completed not ok");
    return false;
  }
  
  return true;
}

}

