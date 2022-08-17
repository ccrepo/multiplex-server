//
// cc_server_pipeline_element.cpp
//

#include "cc_server_pipeline_element.h"

namespace cc {

c_server_pipeline_element::c_server_pipeline_element(int32_t client_fd) : _client_fd(client_fd),
_is_locked(false),
_locked_by_id(0),
_is_close_connection_required(false),
//
_is_completed_failed(false),
_is_datamodel_failed(false),
_is_request_failed(false),
_is_response_failed(false),
//
_is_completed_completed(false),
_is_datamodel_completed(false),
_is_request_completed(false),
_is_response_completed(false),
_request(),
_response() {
}

bool c_server_pipeline_element::lock_with_owner_lockable(uint64_t object_id) {
  if (_is_locked) {
    return false;
  }
  
  if (_is_locked.exchange(true)) {
    return false;
  }
  
  LOG_TRACE("locked locker ", object_id, " ", this);
  
  _locked_by_id = object_id;
  return true;
}

void c_server_pipeline_element::unlock() {
  LOG_TRACE("unlocked locker ", _locked_by_id, " ", this);
  _locked_by_id.store(0);
  _is_locked.store(false);
}

}
