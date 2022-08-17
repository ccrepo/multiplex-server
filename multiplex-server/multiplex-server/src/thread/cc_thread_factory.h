//
// cc_thread_factory.h
//

#ifndef _CC_THREAD_FACTORY_H
#define _CC_THREAD_FACTORY_H

#include <thread>
#include <set>

#include "cc_log.h"
#include "cc_thread.h"

namespace cc {

class c_datamodel_service;
class c_multiplex_network_listener_service;
class c_network_reader_service;
class c_network_writer_service;

template<typename T>
class c_thread_factory final {

  using value_type [[maybe_unused]] = typename std::enable_if<
  std::is_same<c_datamodel_service,                  T>::value ||
  std::is_same<c_multiplex_network_listener_service, T>::value ||
  std::is_same<c_network_writer_service,             T>::value ||
  std::is_same<c_network_reader_service,             T>::value>::type;
  
public:
  static c_thread_factory<T>* factory() {
    static std::unique_ptr<c_thread_factory<T>> _thread_factory(new c_thread_factory<T>());
    return _thread_factory.get();
  }

  ~c_thread_factory() {}

  const std::set<std::shared_ptr<c_thread<T>>> get_threads() const;
  const std::shared_ptr<c_thread<T>> get_thread_prestart() const;
  const std::shared_ptr<c_thread<T>> get_thread() const;

private:
  c_thread_factory() {
  }

  c_thread_factory(const c_thread_factory&) = delete;
  c_thread_factory& operator=(const c_thread_factory&) = delete;

};

}

#endif /* _CC_THREAD_FACTORY_H */

