//
// cc_autocall.h
//

#ifndef _CC_AUTOCALL_H
#define _CC_AUTOCALL_H

#include <atomic>
#include <functional>

namespace cc {

template<typename T>
class c_autocall final {
  
public:
  c_autocall(const std::function<T>& callback) : _callback(callback), _callback_cancelled(false) {
	}

	~c_autocall() {
    if (!_callback_cancelled) {
      _callback();
    }
	}

  void callback_cancelled() {
    _callback_cancelled = true;
  }
  
private:
  const std::function<T>& _callback;
  std::atomic<bool>       _callback_cancelled;
  
private:
  c_autocall(const c_autocall&) = delete;
  c_autocall& operator=(const c_autocall&) = delete;
};

}

#endif /* _CC_AUTOCALL_H */
