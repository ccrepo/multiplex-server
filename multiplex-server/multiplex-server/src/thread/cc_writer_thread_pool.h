//
// cc_writer_thread_pool.h
//

#ifndef _CC_WRITER_THREAD_POOL_H
#define _CC_WRITER_THREAD_POOL_H

#include "cc_log.h"
#include "cc_thread_pool.h"

namespace cc {

class c_network_writer_service;

class c_writer_thread_pool final : public c_thread_pool<c_network_writer_service> {
  
public:
  c_writer_thread_pool() : c_thread_pool<c_network_writer_service>(c_thread_factory<c_network_writer_service>::factory()->get_threads()) {
  }
  
	~c_writer_thread_pool() {
	}

  virtual bool start_pool() override final {
    return c_thread_pool::start_pool();
	}

  virtual bool stop_pool() override final {
    return c_thread_pool::stop_pool();
	}
  
  virtual bool do_thread_maintenance() override final;

private:
  c_writer_thread_pool(const c_writer_thread_pool&) = delete;
  c_writer_thread_pool& operator=(const c_writer_thread_pool&) = delete;
};

}

#endif /* _CC_WRITER_THREAD_POOL_H */
