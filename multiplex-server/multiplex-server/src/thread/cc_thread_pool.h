//
// cc_thread_pool.h
//

#ifndef _CC_THREAD_POOL_H
#define _CC_THREAD_POOL_H

#include <set>

#include "cc_log.h"
#include "cc_thread.h"
#include "cc_attributes.h"
#include "cc_thread_group.h"

namespace cc {

template<typename T>
class c_thread_pool : protected c_attributes_exit {
public:
  
  c_thread_pool(const std::set<std::shared_ptr<c_thread<T>>>& thread_group) : c_attributes_exit(),
  _thread_group(thread_group) {}

	virtual ~c_thread_pool() {}

	virtual bool start_pool() {
    return _thread_group.start_group();
	}

  virtual bool stop_pool() {
    return _thread_group.stop_group();
	}

  virtual bool do_thread_maintenance() {
    return false;
  };
  
protected:
  c_thread_group<T> _thread_group;

private:
  c_thread_pool(const c_thread_pool&) = delete;
  c_thread_pool& operator=(const c_thread_pool&) = delete;
};

}

#endif /* _CC_THREAD_POOL_H */
