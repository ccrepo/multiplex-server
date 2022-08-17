//
// cc_pipe.h 
//

#ifndef _CC_PIPE_H
#define _CC_PIPE_H

#include <deque>
#include <mutex>
#include <atomic>

#include "cc_macros.h"
#include "cc_math.h"
#include "cc_sleep.h"
#include "cc_duration.h"
#include "cc_log.h"

namespace cc {

template<typename T>
class c_pipe final {

public:
  c_pipe(const std::string& label) :
			_elements(), _mutex(), _is_empty(true), _label(label) {
	}

	~c_pipe() {
	}

	bool push_with_timeout(const std::shared_ptr<T>& element) {
		std::unique_lock < std::timed_mutex > lock(_mutex, std::defer_lock);

		if (!lock.try_lock_for(std::chrono::milliseconds(c_math::random_in_range(c_duration::_25_ms, c_duration::_50_ms)))) {
			return false;
		}

		_elements.push_back(element);
    set_is_not_empty();
		return true;
	}
  
  bool push(const std::shared_ptr<T>& element) {
    std::unique_lock < std::timed_mutex > lock(_mutex);
        
    _elements.push_back(element);
    set_is_not_empty();
    return true;
  }

  bool push_with_timeout_n(const std::vector<int32_t>& pending_connections) {
    std::unique_lock < std::timed_mutex > lock(_mutex, std::defer_lock);

    if(pending_connections.empty()) {
      return true;
    }
    
    if (!lock.try_lock_for(std::chrono::milliseconds(c_math::random_in_range(c_duration::_25_ms, c_duration::_50_ms)))) {
      return false;
    }
    
    auto& elements(_elements);
    
    std::for_each(pending_connections.begin(), pending_connections.end(), [&elements](auto& pending_connection) {
      
      if (c_logger_implT<cc::log::c_debug>::is_level_on()) {
        std::shared_ptr<T> element(new T(pending_connection));
        LOG_INFO("created ", element.get());
        elements.push_back(element);
      } else {
        elements.push_back(std::shared_ptr<T>(new T(pending_connection)));
      }
    });
    
    if (_elements.empty()) {
      set_is_empty();
    } else {
      set_is_not_empty();
    }
    
    return true;
  }

  bool do_pipe_maintenance(const std::function<bool(const T*)>& remove_predicate = [](const T*){ return false; }) {
        
    std::unique_lock < std::timed_mutex > lock(_mutex, std::defer_lock);
    
    if (is_empty()) {
      LOG_TRACE("empty flag check ", this);
      return true;
    }
    
    if (!lock.try_lock_for(std::chrono::milliseconds(c_math::random_in_range(c_duration::_25_ms, c_duration::_50_ms)))) {
      LOG_TRACE("try lock failed ", this);
      return false;
    }
    
    if (_elements.empty()) {
      LOG_TRACE(_label, " empty object check ", this);
      return true;
    }
    
    LOG_TRACE(_label, " element count ", _elements.size());
    
    while (!_elements.empty()) {
      
      std::shared_ptr<T> element(*_elements.begin());
      
      if (!element.get()) {
        LOG_TRACE(_label, " removing null pointer");
        _elements.erase(_elements.begin());
        continue;
      }
      
      if (remove_predicate(element.get())) {
        LOG_TRACE(_label, " removing as predicate true ", element.get());
        _elements.erase(_elements.begin());
        continue;
      }
      
      LOG_TRACE(_label, " finished checking ", element.get());
      break;
    }
    
    if (_elements.empty()) {
      set_is_empty();
    } else {
      set_is_not_empty();
    }
    
    return true;
  }
  
  inline void set_is_not_empty() {
    _is_empty.store(false, std::memory_order_seq_cst);
  }

  inline void set_is_empty() {
    _is_empty.store(true, std::memory_order_seq_cst);
  }
  
  inline bool is_empty() const {
    return _is_empty.load(std::memory_order_seq_cst);
  }
  
  bool lock_element_with_timeout(uint64_t object_id,
                                 OUT std::shared_ptr<T>& result,
                                 const std::function<bool(const T*)>& skip_predicate = [](const T*){ return false; } ) {
    
    LOG_TRACE("start ", this);

    std::unique_lock < std::timed_mutex > lock(_mutex, std::defer_lock);
    
    if (is_empty()) {
      LOG_TRACE("empty flag check ", this);
      return false;
    }
    
    if (!lock.try_lock_for(std::chrono::milliseconds(c_math::random_in_range(c_duration::_25_ms, c_duration::_50_ms)))) {
      LOG_TRACE("try lock failed ", this);
      return false;
    }
    
    if (_elements.empty()) {
      LOG_TRACE(_label, " empty object check ", this);
      return false;
    }

    LOG_TRACE(_label, " element count ", _elements.size());
    
    for (std::shared_ptr<T> element : _elements) {
      
      if (!element.get()) {
        LOG_TRACE(_label, " skipping null element");
        continue;
      }
      
      if (skip_predicate(element.get())) {
        LOG_TRACE(_label, " skipping as predicate on element is true ", element.get());
        continue;
      }

      if (!element.get()->lock_with_owner_lockable(object_id)) {
        LOG_TRACE(_label, " skipping lock failed ", element.get());
        continue;
      }

      result = element;
      LOG_TRACE(_label, " ok ", element.get());
      return true;
    }

    LOG_TRACE(_label, " no element found ", this);
    return false;
  }
    
private:
  const std::string              _label;
	std::deque<std::shared_ptr<T>> _elements;
	std::timed_mutex               _mutex;
	std::atomic<bool>              _is_empty;

private:
  c_pipe(const c_pipe&) = delete;
  c_pipe& operator=(const c_pipe&) = delete;
};

}

#endif /* _CC_PIPE_H */
