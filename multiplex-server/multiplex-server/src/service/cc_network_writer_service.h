//
// cc_network_writer_service.h
//

#ifndef _CC_NETWORK_WRITER_SERVICE_H
#define _CC_NETWORK_WRITER_SERVICE_H

#include <memory>

#include "cc_sleep.h"
#include "cc_log.h"
#include "cc_attributes.h"
#include "cc_macros.h"
#include "cc_thread_service.h"
#include "cc_autocall.h"

namespace cc {

class c_server_pipeline_element;

class c_network_writer_service final : private c_attributes_exit,
private c_attributes_alive,
protected c_attributes_id,
public c_thread_service {
  
public:
  enum NETWORK_WRITER_STATUS { WRITER_STATUS_NONE,
    WRITER_STATUS_OK,
    WRITER_STATUS_NETWORK_ERROR,
    WRITER_STATUS_SYSTEM_ERROR,
    WRITER_STATUS_SYSTEM_EXIT
  };
  
public:
  c_network_writer_service();
  
  ~c_network_writer_service() {}
  
  virtual bool start_thread_service() override final {
    return service_loop();
  }
  
  virtual bool stop_thread_service() override final;
  
  virtual bool is_attribute_exit_instance() const override final {
    return c_attributes_exit::is_attribute_exit_instance();
  }
  
  virtual bool is_attribute_alive_instance() const override final {
    return c_attributes_alive::is_attribute_alive_instance();
  }
  
  virtual void set_instance_exit_flag() override final {
    c_attributes_exit::set_instance_exit_flag();
  }
  
private:
  const int32_t _network_buffer_size;
  
private:
  bool service();
  
  virtual bool service_loop() override;
  
  NETWORK_WRITER_STATUS service_write(std::shared_ptr<c_server_pipeline_element>& server_task);
  
  bool publish_to_pipe(NETWORK_WRITER_STATUS status,
                       const std::shared_ptr<c_server_pipeline_element>& server_task,
                       OUT c_autocall<void()>& autocall_fail);
  
  bool publish_to_completed_pipe(NETWORK_WRITER_STATUS status,
                                 const std::shared_ptr<c_server_pipeline_element>& server_task,
                                 OUT c_autocall<void()>& autocall_fail);
  
  bool publish_to_pipe_not_ok(NETWORK_WRITER_STATUS status,
                              const std::shared_ptr<c_server_pipeline_element>& server_task);
  
private:
  c_network_writer_service(const c_network_writer_service&) = delete;
  c_network_writer_service& operator=(const c_network_writer_service&) = delete;
};

}

#endif /* _CC_NETWORK_WRITER_SERVICE_H */
