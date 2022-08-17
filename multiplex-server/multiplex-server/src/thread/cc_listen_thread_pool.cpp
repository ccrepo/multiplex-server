//
// cc_listen_thread_pool.cpp
//

#include <algorithm>

#include "cc_log.h"
#include "cc_sleep.h"
#include "cc_listen_thread_pool.h"
#include "cc_thread_factory.h"

namespace cc {

bool c_listen_thread_pool::do_thread_maintenance() {
  
  if (is_attribute_exit_any()) {
    LOG_DEBUG("exit signalled");
    return true;
  }
  
  if (!_thread_group.do_thread_group_maintenance()) {
    LOG_DEBUG("failed maintenance");
    return false;
  }
  
  return true;
}

}
