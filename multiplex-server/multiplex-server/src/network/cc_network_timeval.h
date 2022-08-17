//
// cc_network_timeval.h
//

#ifndef _CC_NETWORK_TIMEVAL_H
#define _CC_NETWORK_TIMEVAL_H

#include <memory>
#include <cstring>

#include <sys/time.h>

namespace cc {

class c_network_timeval final {

public:
  c_network_timeval(int32_t network_connection_read_timeout_ms) {
        memset(&_tv, 0, sizeof(::timeval));
    _tv.tv_sec = network_connection_read_timeout_ms / 1000;
    _tv.tv_usec = (network_connection_read_timeout_ms % 1000) * 1000;
  }

  ~c_network_timeval() {
	}

  timeval* timeval() {
    return &_tv;
  }

private:
  struct timeval _tv;
  
private:
  c_network_timeval(const c_network_timeval&) = delete;
  c_network_timeval& operator=(const c_network_timeval&) = delete;
};

} 

#endif /* _CC_NETWORK_TIMEVAL_H */

