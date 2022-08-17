//
// cc_server.cpp
//

#include "cc_server.h"
#include "cc_server_pipeline.h"

namespace cc {

bool c_server::start_process() {

  if (!_listen_thread_pool->start_pool()) {
    LOG_FATAL("could not start listener pool");
    return false;
  }

  if (!_reader_thread_pool->start_pool()) {
    LOG_FATAL("could not start reader pool");
    return false;
  }

  if (!_datamodel_thread_pool->start_pool()) {
    LOG_FATAL("could not start datamodel pool");
    return false;
  }

  if (!_writer_thread_pool->start_pool()) {
    LOG_FATAL("could not start writer pool");
    return false;
  }
  
	return true;
}

bool c_server::stop_process() {
  
  bool result(true);
  
  if (!_listen_thread_pool->stop_pool()) {
    LOG_ERROR("could not stop listener pool");
    result = false;
  }

  if (!_reader_thread_pool->stop_pool()) {
    LOG_ERROR("could not stop reader pool");
    result = false;
  }

  if (!_datamodel_thread_pool->stop_pool()) {
    LOG_ERROR("could not stop datamodel pool");
    result = false;
  }
  
  if (!_writer_thread_pool->stop_pool()) {
    LOG_ERROR("could not stop writer pool");
    result = false;
  }

  return result;
}

c_server::c_server() :
_listen_thread_pool(new c_listen_thread_pool()),
_datamodel_thread_pool(new c_datamodel_thread_pool()),
_reader_thread_pool(new c_reader_thread_pool()),
_writer_thread_pool(new c_writer_thread_pool())
{}

bool c_server::do_process_maintenance() {
  bool result(true);

  if(is_attribute_exit_any()) {
    return result;
  }
  
  if (!_listen_thread_pool->do_thread_maintenance()) {
    LOG_ERROR("listener pool maintenance failed");
    result = false;
  }

  if(is_attribute_exit_any()) {
    return result;
  }
  
  if (!_reader_thread_pool->do_thread_maintenance()) {
    LOG_ERROR("reader pool maintenance failed");
    result = false;
  }

  if(is_attribute_exit_any()) {
    return result;
  }

  if (!_datamodel_thread_pool->do_thread_maintenance()) {
    LOG_ERROR("datamodel pool maintenance failed");
    result = false;
  }
  
  if(is_attribute_exit_any()) {
    return result;
  }

  if (!_writer_thread_pool->do_thread_maintenance()) {
    LOG_ERROR("writer pool maintenance failed");
    result = false;
  }
  
  if(is_attribute_exit_any()) {
    return result;
  }
  
  if (!c_server_pipeline::singleton()->do_pipeline_maintenance()) {
    LOG_ERROR("pipeline maintenance failed");
    result = false;
  }
  
  if(is_attribute_exit_any()) {
    return result;
  }

  return result;
}

bool c_server::is_attribute_exit_any() const {
  if (c_attributes_exit::is_attribute_exit_any()) {
    return true;
  }
  
  return false;
}

}
 
