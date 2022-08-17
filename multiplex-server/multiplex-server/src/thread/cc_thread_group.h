//
// cc_thread_group.h
//

#ifndef _CC_THREAD_GROUP_H
#define _CC_THREAD_GROUP_H

#include <set>

#include "cc_log.h"
#include "cc_thread.h"
#include "cc_collections.h"
#include "cc_thread_factory.h"

namespace cc {

template <typename T>
class c_thread_group final {
public:
  
  c_thread_group(const std::set<std::shared_ptr<c_thread<T>>>& thread_group) : _active_threads(thread_group),
  _initial_size(_active_threads.size()) {
  }

	~c_thread_group() {}

	bool start_group() {
    
    if (_active_threads.empty()) {
      return false;
    }

    bool result { c_collections::collection_to_bool(_active_threads.begin(),
                                                 _active_threads.end(), [](auto& thr) { return thr->start_thread(); } ) };

    return result;
	}

  bool stop_group() {

    if (_active_threads.empty()) {
      return false;
    }

    bool result { c_collections::collection_to_bool(_active_threads.begin(),
                                                _active_threads.end(), [](auto& thr) { return thr->stop_thread(); } ) };
    
    return result;
	}

  inline size_t initial_size() const {
    return _initial_size;
  }

  bool do_thread_group_maintenance() {
    
    _dead_threads.clear();
    
    for (auto iter = _exiting_threads.begin(); iter != _exiting_threads.end(); ) {
      const std::shared_ptr<c_thread<T>>& ptr(*iter);
      
      if (!ptr->is_attribute_alive_instance()) {
        LOG_DEBUG("move exiting thread ", ptr.get(), " to dead");
        _dead_threads.insert(ptr);
        iter = _exiting_threads.erase(iter);
      }
      else {
        ++iter;
      }
    }
    
    for (auto iter = _active_threads.begin(); iter != _active_threads.end(); ) {
      const std::shared_ptr<c_thread<T>>& ptr(*iter);
      
      if (!ptr->is_attribute_alive_instance()) {
        LOG_DEBUG("move active thread ", ptr.get(), "to dead");
        _dead_threads.insert(ptr);
        iter = _active_threads.erase(iter);
      }
      else if (ptr->is_attribute_exit_instance()) {
        LOG_DEBUG("move active thread ", ptr.get(), " to exiting");
        _exiting_threads.insert(ptr);
        iter = _active_threads.erase(iter);
      }
      else {
        ++iter;
      }
    }
    
    std::generate_n(std::inserter(_active_threads, _active_threads.begin()),
                    this->_initial_size - _active_threads.size(),
                    std::bind(&c_thread_factory<T>::get_thread_prestart, c_thread_factory<T>::factory()));
    
    return true;
  }
  
protected:
	std::set<std::shared_ptr<c_thread<T>>> _active_threads;
  std::set<std::shared_ptr<c_thread<T>>> _dead_threads;
  std::set<std::shared_ptr<c_thread<T>>> _exiting_threads;

private:
  const size_t _initial_size;
  
private:
  c_thread_group(const c_thread_group&) = delete;
  c_thread_group& operator=(const c_thread_group&) = delete;
};

}

#endif /* _CC_THREAD_GROUP_H */
