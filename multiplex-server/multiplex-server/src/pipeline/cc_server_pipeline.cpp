//
// cc_server_pipeline.cpp
//

#include "cc_server_pipeline.h"

namespace cc {

c_server_pipeline::c_server_pipeline() : _request_pipe("connect"),
_datamodel_pipe("request"),
_response_pipe("datamodel"),
_completed_pipe("response") {
}

bool c_server_pipeline::publish_connect_n(const std::vector<int32_t>& pending_connections) {
  return _request_pipe.push_with_timeout_n(pending_connections);
}

bool c_server_pipeline::get_next_request(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element) {
  return _request_pipe.lock_element_with_timeout(object_id, element, get_request_completed_predicate());
}

bool c_server_pipeline::get_next_datamodel(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element) {
  return _datamodel_pipe.lock_element_with_timeout(object_id, element, get_datamodel_completed_predicate());
}

bool c_server_pipeline::get_next_response(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element) {
  return _response_pipe.lock_element_with_timeout(object_id, element, get_response_completed_predicate());
}

bool c_server_pipeline::get_next_completed(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element) {
  return _completed_pipe.lock_element_with_timeout(object_id, element, get_completed_completed_predicate());
}

//

bool c_server_pipeline::publish_to_datamodel_pipe(uint64_t object_id, const std::shared_ptr<c_server_pipeline_element>& element) {
  if (element->locked_by_id() != object_id) {
    return false;
  }
  
  if (!_datamodel_pipe.push(element)) {
    return false;
  }
  
  element->mark_as_request_completed();
  return true;
}

bool c_server_pipeline::publish_to_response_pipe(uint64_t object_id, const std::shared_ptr<c_server_pipeline_element>& element) {
  if (element->locked_by_id() != object_id) {
    return false;
  }
  
  if (!_response_pipe.push(element)) {
    return false;
  }

  element->mark_as_datamodel_completed();
  return true;
}

bool c_server_pipeline::publish_to_completed_pipe(uint64_t object_id, const std::shared_ptr<c_server_pipeline_element>& element) {
  if (element->locked_by_id() != object_id) {
    return false;
  }
  
  if (!_completed_pipe.push(element)) {
    return false;
  }
  
  element->mark_as_response_completed();
  return true;
}

//

bool c_server_pipeline::do_pipeline_maintenance(const std::string& description,
                                                const std::function<bool(const c_server_pipeline_element*)>& remove_predicate,
                                                c_pipe<c_server_pipeline_element>& tasks,
                                                int32_t& failed_count,
                                                bool& failure_limit_exceeded) {
    
  if (!tasks.do_pipe_maintenance(remove_predicate)) {
    
    ++failed_count;
    
    if (failed_count > c_config::singleton()->maintenance_warning_count()) {
      LOG_WARN(description, " do maintenance failed ", failed_count, "times - resetting count");
      failure_limit_exceeded = true;
      failed_count = 0;
    }
  } else {
    failed_count = 0;
    return true;
  }

  LOG_TRACE("not ok");
  return false;
}

bool c_server_pipeline::do_pipeline_maintenance() {

  static int32_t connect_failed_count(0);
  static int32_t request_failed_count(0);
  static int32_t datamodel_failed_count(0);
  static int32_t response_failed_count(0);
  
  bool failure_limit_exceeded(false);
  bool result(true);
  
  result = do_pipeline_maintenance("connect pipe to request pipe",
                                   get_request_completed_predicate(),
                                   _request_pipe,
                                   connect_failed_count,
                                   failure_limit_exceeded) && result;
  
  result = do_pipeline_maintenance("request pipe to datamodel pipe",
                                   get_datamodel_completed_predicate(),
                                   _datamodel_pipe,
                                   request_failed_count,
                                   failure_limit_exceeded) && result;
  
  result = do_pipeline_maintenance("datamodel pipe to response pipe",
                                   get_response_completed_predicate(),
                                   _response_pipe,
                                   datamodel_failed_count,
                                   failure_limit_exceeded) && result;
  
  result = do_pipeline_maintenance("response pipe to completed pipe",
                                   get_completed_completed_predicate(),
                                   _completed_pipe,
                                   response_failed_count,
                                   failure_limit_exceeded) && result;
  
  if (failure_limit_exceeded) {
    LOG_ERROR("failure limit exceeded");
  }

  return result;
}

} 


