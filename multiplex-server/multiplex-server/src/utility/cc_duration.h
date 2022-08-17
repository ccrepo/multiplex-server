//
// cc_duration.h
//

#ifndef _CC_DURATION_H
#define _CC_DURATION_H

namespace cc {

class c_duration final {

public:
  static constexpr const uint32_t _10_ms { 10 };
  static constexpr const uint32_t _25_ms { 25 };
  static constexpr const uint32_t _50_ms { 50 };
  static constexpr const uint32_t _75_ms { 75 };
  static constexpr const uint32_t _100_ms { 100 };
  static constexpr const uint32_t _150_ms { 150 };
  static constexpr const uint32_t _250_ms { 250 };
  static constexpr const uint32_t _500_ms { 500 };
  static constexpr const uint32_t _1000_ms { 1000 };
  static constexpr const uint32_t _2000_ms { 2000 };
  static constexpr const uint32_t _5000_ms { 5000 };
  static constexpr const uint32_t _10000_ms { 10000 };
  
private:
  c_duration() = delete;
  c_duration(c_duration&) = delete;
  c_duration& operator=(c_duration&) = delete;
	~c_duration() = delete;
};

}

#endif /* _CC_DURATION_H */
