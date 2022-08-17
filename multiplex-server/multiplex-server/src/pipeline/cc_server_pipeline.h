//
// cc_server_pipeline.h
//

#ifndef _CC_SERVER_PIPELINE_H
#define _CC_SERVER_PIPELINE_H

#include <memory>
#include <deque>

#include "cc_server_pipeline_element.h"
#include "cc_pipe.h"
#include "cc_log.h"

namespace cc {

class c_server_pipeline final : public std::enable_shared_from_this<c_server_pipeline> {

public:
  static c_server_pipeline* singleton() {
    static std::shared_ptr<c_server_pipeline> server_pipeline(new c_server_pipeline());
    return server_pipeline.get();
  }

  ~c_server_pipeline() {}
  
  bool publish_connect_n(const std::vector<int32_t>& pending_connections);

  bool get_next_request(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element);
  bool get_next_datamodel(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element);
  bool get_next_response(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element);
  bool get_next_completed(uint64_t object_id, std::shared_ptr<c_server_pipeline_element>& element);

  bool publish_to_datamodel_pipe(uint64_t object_id, const std::shared_ptr<c_server_pipeline_element>& element);
  bool publish_to_response_pipe(uint64_t object_id, const std::shared_ptr<c_server_pipeline_element>& element);
  bool publish_to_completed_pipe(uint64_t object_id, const std::shared_ptr<c_server_pipeline_element>& element);

  bool do_pipeline_maintenance();

private:
  bool do_pipeline_maintenance(const std::string& description,
                      const std::function<bool(const c_server_pipeline_element*)>& remove_predicate,
                      c_pipe<c_server_pipeline_element>& tasks,
                      int32_t& failed_count,
                      bool& warned);
  
private:
  c_pipe<c_server_pipeline_element> _request_pipe;
  c_pipe<c_server_pipeline_element> _datamodel_pipe;
  c_pipe<c_server_pipeline_element> _response_pipe;
  c_pipe<c_server_pipeline_element> _completed_pipe;
  
private:
  // request
  std::function<bool(const c_server_pipeline_element*)> get_request_completed_predicate() {
    return [] (const c_server_pipeline_element* element){ return c_server_pipeline::is_request_completed_predicate(element); };
  }
  
  static bool is_request_completed_predicate(const c_server_pipeline_element* element, bool ignore_completed=true) {
    return (ignore_completed && element->is_request_completed()) ||
    element->is_request_failed();
  }

  // datamodel
  std::function<bool(const c_server_pipeline_element*)> get_datamodel_completed_predicate() {
    return [] (const c_server_pipeline_element* element){ return c_server_pipeline::is_datamodel_completed_predicate(element); };
  }
  
  static bool is_datamodel_completed_predicate(const c_server_pipeline_element* element, bool ignore_completed=true) {
    return (ignore_completed && element->is_datamodel_completed()) ||
    element->is_datamodel_failed() ||
    is_request_completed_predicate(element, false);
  }
  
  // response
  std::function<bool(const c_server_pipeline_element*)> get_response_completed_predicate() {
    return [] (const c_server_pipeline_element* element){ return c_server_pipeline::is_response_completed_predicate(element); };
  }
  
  static bool is_response_completed_predicate(const c_server_pipeline_element* element, bool ignore_completed=true) {
    return (ignore_completed && element->is_response_completed()) ||
    element->is_response_failed() ||
    is_datamodel_completed_predicate(element, false);
  }
  
  // completed
  std::function<bool(const c_server_pipeline_element*)> get_completed_completed_predicate() {
    return [] (const c_server_pipeline_element* element){ return c_server_pipeline::is_completed_completed_predicate(element); };
  }
  
  static bool is_completed_completed_predicate(const c_server_pipeline_element* element) {
    return element->is_completed_completed();
  }

private:
  c_server_pipeline();
  
private:
  c_server_pipeline& operator=(const c_server_pipeline&) = delete;
  c_server_pipeline(const c_server_pipeline&) = delete;
};

}
 
#endif /* _CC_SERVER_PIPELINE_H */

