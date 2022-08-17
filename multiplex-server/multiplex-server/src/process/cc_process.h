//
// cc_process.h
//

#ifndef _CC_PROCESS_H
#define _CC_PROCESS_H

#include <atomic>
#include <thread>

#include "cc_system.h"
#include "cc_log.h"
#include "cc_sleep.h"
#include "cc_thread.h"
#include "cc_attributes.h"

namespace cc {

class c_process : protected c_attributes_exit {

public:
  virtual bool start_process() {
    return false;
  };
  
  virtual bool stop_process() {
    return false;
  };
  
  virtual bool is_attribute_exit_any() const {
    return false;
  };
  
  virtual bool do_process_maintenance() {
    return false;
  };

protected:
	c_process() : c_attributes_exit() {}

	virtual ~c_process() {}

private:
	c_process(const c_process&) = delete;
	c_process& operator=(const c_process&) = delete;
};

} 

#endif /* _CC_PROCESS_H */
