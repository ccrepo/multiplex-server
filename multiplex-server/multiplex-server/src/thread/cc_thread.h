//
// cc_thread.h
//

#ifndef _CC_THREAD_H
#define _CC_THREAD_H

#include <thread>

#include "cc_log.h"
#include "cc_attributes.h"
#include "cc_thread_service.h"

namespace cc {

template<typename T>
class c_thread_factory;

template<typename T>
class c_thread {

public:
  virtual ~c_thread() {
    if (_thread.get() && _thread->joinable()) {
      _thread->join();
    }
  }

  bool start_thread() {

    if (!_thread.get()) {
      _thread.reset(new std::thread(&c_thread_service::run, _thread_service.get()));
      return true;
    }
    
    return false;
  }
  
  bool stop_thread() {
    
    if (!_thread_service->stop_thread_service()) {
      LOG_WARN("stop thread service error ", this);
      return false;
    }
    
    return true;
  }

  bool is_attribute_alive_instance() const {
    return _thread_service->is_attribute_alive_instance();
  }
  
  bool is_attribute_exit_instance() const {
    return _thread_service->is_attribute_exit_instance();
  }
  
  void set_instance_exit_flag() {
    _thread_service->set_instance_exit_flag();
  }

private:
  friend class c_thread_factory<T>;
  
  c_thread() : _thread_service(new T()) {}

protected:
  std::unique_ptr<std::thread> _thread;
  std::unique_ptr<c_thread_service> _thread_service;

private:
  c_thread(const c_thread&) = delete;
  c_thread& operator=(const c_thread&) = delete;

};

}

#endif /* _CC_THREAD_H */
