//
// cc_server.h
//

#ifndef _CC_SERVER_H
#define _CC_SERVER_H

#include "cc_log.h"
#include "cc_process.h"
#include "cc_listen_thread_pool.h"
#include "cc_datamodel_thread_pool.h"
#include "cc_reader_thread_pool.h"
#include "cc_writer_thread_pool.h"

namespace cc {

class c_process_factory;

class c_server final : public c_process {

public:
  ~c_server() {}

  virtual bool start_process() override final;
  virtual bool stop_process() override final;
  virtual bool do_process_maintenance() override final;
  virtual bool is_attribute_exit_any() const override final;
  
private:
	friend c_process_factory;

private:
  const std::unique_ptr<c_listen_thread_pool>    _listen_thread_pool;
  const std::unique_ptr<c_datamodel_thread_pool> _datamodel_thread_pool;
  const std::unique_ptr<c_reader_thread_pool>    _reader_thread_pool;
  const std::unique_ptr<c_writer_thread_pool>    _writer_thread_pool;
  
private:
	c_server();

private:
	c_server(const c_server&) = delete;
	c_server& operator=(const c_server&) = delete;
};

}

#endif // _CC_SERVER_H

 
