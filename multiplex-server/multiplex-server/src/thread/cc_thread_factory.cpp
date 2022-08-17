//
// cc_thread_factory.cpp
//

#include "cc_log.h"
#include "cc_thread_factory.h"
#include "cc_datamodel_service.h"
#include "cc_network_reader_service.h"
#include "cc_network_writer_service.h"
#include "cc_multiplex_network_listener_service.h"

namespace cc {

using c_listen_thread     = c_thread<c_multiplex_network_listener_service>;
using c_listen_factory    = c_thread_factory<c_multiplex_network_listener_service>;
using c_read_thread       = c_thread<c_network_reader_service>;
using c_read_factory      = c_thread_factory<c_network_reader_service>;
using c_datamodel_thread  = c_thread<c_datamodel_service>;
using c_datamodel_factory = c_thread_factory<c_datamodel_service>;
using c_write_thread      = c_thread<c_network_writer_service>;
using c_write_factory     = c_thread_factory<c_network_writer_service>;

// listener
template<>
const std::shared_ptr<c_listen_thread> c_listen_factory::get_thread() const {
  return std::shared_ptr<c_listen_thread>((std::shared_ptr<c_listen_thread>(new c_listen_thread())));
}

template<>
const std::set<std::shared_ptr<c_listen_thread>> c_listen_factory::get_threads() const {
  std::set<std::shared_ptr<c_listen_thread>> threads;
  std::generate_n(std::inserter(threads, threads.begin()),
                  1,
                  // disabled c_config::singleton()->capacity_listener_pool_size(),
                  std::bind(&c_listen_factory::get_thread, this));
  return threads;
}

template<>
const std::shared_ptr<c_listen_thread> c_listen_factory::get_thread_prestart() const {
  auto ptr(c_listen_factory::get_thread());
  ptr->start_thread();
  return ptr;
}

// reader
template<>
const std::shared_ptr<c_read_thread> c_read_factory::get_thread() const {
  return std::shared_ptr<c_read_thread>(std::shared_ptr<c_read_thread>(new c_read_thread()));
}

template<>
const std::set<std::shared_ptr<c_read_thread>> c_read_factory::get_threads() const {
  std::set<std::shared_ptr<c_read_thread>> threads;
  std::generate_n(std::inserter(threads, threads.begin()),
                  c_config::singleton()->capacity_reader_pool_size(),
                  std::bind(&c_read_factory::get_thread, this));
  return threads;
}

template<>
const std::shared_ptr<c_read_thread> c_read_factory::get_thread_prestart() const {
  auto ptr(c_read_factory::get_thread());
  ptr->start_thread();
  return ptr;
}

// datamodel
template<>
const std::shared_ptr<c_datamodel_thread> c_datamodel_factory::get_thread() const {
  return std::shared_ptr<c_datamodel_thread>(std::shared_ptr<c_datamodel_thread>(new c_datamodel_thread()));
}

template<>
const std::set<std::shared_ptr<c_datamodel_thread>> c_datamodel_factory::get_threads() const {
  std::set<std::shared_ptr<c_datamodel_thread>> threads;
  std::generate_n(std::inserter(threads, threads.begin()),
                  c_config::singleton()->capacity_datamodel_pool_size(),
                  std::bind(&c_datamodel_factory::get_thread, this));
  return threads;
}

template<>
const std::shared_ptr<c_datamodel_thread> c_datamodel_factory::get_thread_prestart() const {
  auto ptr(c_datamodel_factory::get_thread());
  ptr->start_thread();
  return ptr;
}

// writer
template<>
const std::shared_ptr<c_write_thread> c_write_factory::get_thread() const {
  return std::shared_ptr<c_write_thread>(std::shared_ptr<c_write_thread>(new c_write_thread()));
}

template<>
const std::set<std::shared_ptr<c_write_thread>> c_write_factory::get_threads() const {
  std::set<std::shared_ptr<c_write_thread>> threads;
  std::generate_n(std::inserter(threads, threads.begin()),
                  c_config::singleton()->capacity_writer_pool_size(),
                  std::bind(&c_write_factory::get_thread, this));
  return threads;
}

template<>
const std::shared_ptr<c_write_thread> c_write_factory::get_thread_prestart() const {
  auto ptr(c_write_factory::get_thread());
  ptr->start_thread();
  return ptr;
}

}

