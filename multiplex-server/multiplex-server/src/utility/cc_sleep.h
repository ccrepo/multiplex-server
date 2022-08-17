//
// cc_sleep.h
//

#ifndef _CC_SLEEP_H
#define _CC_SLEEP_H

#include <chrono>
#include <thread>

#include "cc_math.h"
#include "cc_duration.h"

namespace cc {

class c_sleep final {

private:
  inline static void sleep(uint32_t duration) {
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
  }
    
public:
	inline static void sleep_10_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_10_ms);
	}

	inline static void sleep_25_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_25_ms);
	}

	inline static void sleep_50_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_50_ms);
	}

  inline static void sleep_75_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_75_ms);
  }

	inline static void sleep_100_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_100_ms);
	}

	inline static void sleep_150_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_150_ms);
	}

	inline static void sleep_250_ms() {
    std::this_thread::yield();
    c_sleep::c_sleep::sleep(c_duration::_250_ms);
	}

	inline static void sleep_500_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_500_ms);
	}

	inline static void sleep_1000_ms() {
    std::this_thread::yield();
    c_sleep::sleep(c_duration::_1000_ms);
	}

	// general purpose
	inline static void sleep_for(uint32_t ms) {
    std::this_thread::yield();
    c_sleep::sleep(ms);
	}

	inline static void sleep_for_random_in_range(uint32_t lower_ms, uint32_t upper_ms) {
    std::this_thread::yield();
    c_sleep::sleep(c_math::random_in_range(lower_ms, upper_ms));
	}

private:
	c_sleep() = delete;
	c_sleep(const c_sleep&) = delete;
	c_sleep& operator=(const c_sleep&) = delete;
	~c_sleep() = delete;
};

}

#endif /* _CC_SLEEP_H */
