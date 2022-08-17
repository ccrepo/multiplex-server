//
// cc_network_reader_service.h
//

#ifndef _CC_NETWORK_READER_SERVICE_H
#define _CC_NETWORK_READER_SERVICE_H

#include <vector>

#include "cc_sleep.h"
#include "cc_log.h"
#include "cc_attributes.h"
#include "cc_macros.h"
#include "cc_thread_service.h"
#include "cc_autocall.h"

namespace cc {

class c_server_pipeline_element;

class c_network_reader_service final : private c_attributes_exit,
private c_attributes_alive,
protected c_attributes_id,
public c_thread_service {

public:
  enum NETWORK_READER_STATUS { READER_STATUS_NONE,
    READER_STATUS_OK,
    READER_STATUS_MESSAGE_TOO_LONG,
    READER_STATUS_MESSAGE_BIGGER_THAN_HEADER_VALUE,
    READER_STATUS_HEADER_INVALID,
    READER_STATUS_HEADER_MISSING,
    READER_STATUS_NETWORK_ERROR,
    READER_STATUS_NETWORK_TOO_MANY_READS,
    READER_STATUS_SYSTEM_ERROR,
    READER_STATUS_CONNECTION_CLOSED,
    READER_STATUS_SYSTEM_EXIT
  };
 
public:
  c_network_reader_service();

	~c_network_reader_service() {}

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
	const std::unique_ptr<char> _network_buffer;
  std::ostringstream _result_buffer;
	
private:
  bool service();
  
  virtual bool service_loop() override;
  
  NETWORK_READER_STATUS service_read(std::shared_ptr<c_server_pipeline_element>& server_task);
  
  bool publish_to_pipe(NETWORK_READER_STATUS status,
                       const std::shared_ptr<c_server_pipeline_element>& server_task,
                       OUT c_autocall<void()>& autocall_fail);
  
  bool publish_to_datamodel_pipe(NETWORK_READER_STATUS status,
                                 const std::shared_ptr<c_server_pipeline_element>& server_task,
                                 OUT c_autocall<void()>& autocall_fail);
  
  bool publish_to_pipe_not_ok(NETWORK_READER_STATUS status,
                              const std::shared_ptr<c_server_pipeline_element>& server_task);

private:
  c_network_reader_service(const c_network_reader_service&) = delete;
  c_network_reader_service& operator=(const c_network_reader_service&) = delete;
};

} 
 
#endif /* _CC_NETWORK_READER_SERVICE_H */

