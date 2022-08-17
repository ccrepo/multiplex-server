//
// cc_thread_service.h
//

#ifndef _CC_THREAD_SERVICE_H
#define _CC_THREAD_SERVICE_H

namespace cc {

class c_thread_service {

public:
  c_thread_service() {}
  
  virtual ~c_thread_service() {}

  virtual bool start_thread_service() {
    return false;
  };
  
  virtual bool stop_thread_service() {
    return false;
  }
  
  virtual bool is_attribute_alive_instance() const {
    return false;
  };
  
  virtual bool is_attribute_exit_instance() const {
    return false;
  };
  
  virtual void set_instance_exit_flag() {};
  
  void run() {
    this->start_thread_service();
  }
  
private:
  virtual bool service_loop() {
    return false;
  }
  
private:
  c_thread_service(const c_thread_service&) = delete;
  c_thread_service& operator=(const c_thread_service&) = delete;

};

}

#endif /* _CC_THREAD_SERVICE_H */
