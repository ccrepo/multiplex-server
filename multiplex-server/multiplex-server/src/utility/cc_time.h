//
// cc_time.h
//

#ifndef _CC_TIME_H
#define _CC_TIME_H

#include <ctime>
#include <iomanip>
#include <chrono>
using namespace std::chrono_literals;

namespace cc {

class c_time final {

private:
  static auto epoch() {
    static const auto e(std::chrono::system_clock::from_time_t(0));
    return e;
  }

  inline static auto now() {
    return std::chrono::system_clock::now();
  }

public:
	inline static long long ts_EPOCH_MS() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(now() - epoch()).count();
	}

	inline static long long ts_EPOCH_US() {
		return std::chrono::duration_cast<std::chrono::microseconds>(now() - epoch()).count();
	}

	inline static time_t time_t_SECONDS() {
		return std::time(NULL);
	}

private:
	c_time() = delete;
	c_time(c_time&) = delete;
	c_time& operator=(c_time&) = delete;
	~c_time() = delete;
};

}

#endif /* _CC_TIME_H */
