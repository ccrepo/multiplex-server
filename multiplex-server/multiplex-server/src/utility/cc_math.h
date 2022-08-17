//
// cc_math.h
//

#ifndef _CC_MATH_H
#define _CC_MATH_H

#include <random>

namespace cc {

class c_math final {

public:
	inline static uint32_t random_in_range(uint32_t l, uint32_t u) {

    static std::random_device device;
    static std::mt19937 generator(device());

    if (u <= l) {
			return l;
		}

		std::uniform_int_distribution<> distr(l, u);
		return distr(generator);
	}

  template<typename T>
  inline static T min(const T& a, const T& b) {
    using value_type [[maybe_unused]] = typename std::enable_if<
    std::is_same<int64_t, T>::value ||
    std::is_same<int32_t, T>::value ||
    std::is_same<int16_t, T>::value>::type;

    return (a < b) ? a : b;
  }

  template<typename T>
  inline static T max(const T& a, const T& b) {
    using value_type [[maybe_unused]] = typename std::enable_if<
    std::is_same<int64_t, T>::value ||
    std::is_same<int32_t, T>::value ||
    std::is_same<int16_t, T>::value>::type;

    return (a > b) ? a : b;
  }

private:
	c_math() = delete;
	c_math(const c_math&) = delete;
	c_math& operator=(const c_math&) = delete;
	~c_math() = delete;
};

}

#endif /* _CC_MATH_H */
