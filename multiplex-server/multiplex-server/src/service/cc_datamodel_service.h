//
// cc_datamodel_service.h
//

#ifndef _CC_DATAMODEL_SERVICE_H
#define _CC_DATAMODEL_SERVICE_H

#include <vector>

#include "cc_sleep.h"
#include "cc_log.h"
#include "cc_attributes.h"
#include "cc_macros.h"
#include "cc_thread_service.h"
#include "cc_autocall.h"

namespace cc {

class c_server_pipeline_element;

class c_datamodel_service final : private c_attributes_exit,
private c_attributes_alive,
protected c_attributes_id,
public c_thread_service {

public:
  enum DATAMODEL_STATUS { DATAMODEL_STATUS_NONE,
    DATAMODEL_STATUS_OK,
    DATAMODEL_STATUS_NOT_OK };
  
public:
  c_datamodel_service();

	~c_datamodel_service() {}

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
  std::ostringstream _result_buffer;

private:
  bool service();
  
  virtual bool service_loop() override;
  
  DATAMODEL_STATUS service_datamodel(std::shared_ptr<c_server_pipeline_element>& server_task);
  
  bool publish_to_pipe(DATAMODEL_STATUS status,
                       const std::shared_ptr<c_server_pipeline_element>& server_task,
                       OUT c_autocall<void()>& autocall_fail);
  
  bool publish_to_response_pipe(DATAMODEL_STATUS status,
                                const std::shared_ptr<c_server_pipeline_element>& server_task,
                                OUT c_autocall<void()>& autocall_fail);
  
  bool publish_to_pipe_not_ok(DATAMODEL_STATUS status,
                              const std::shared_ptr<c_server_pipeline_element>& server_task);

private:
  c_datamodel_service(const c_datamodel_service&) = delete;
  c_datamodel_service& operator=(const c_datamodel_service&) = delete;
};

} 
 
#endif /* _CC_DATAMODEL_SERVICE_H */

