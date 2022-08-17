//
// cc_autoset.h
//

#ifndef _CC_AUTOSET_H
#define _CC_AUTOSET_H

#include <atomic>

namespace cc {

template<typename T>
class c_autoset final {
  
	using value_type [[maybe_unused]] = typename std::enable_if<
  std::is_same<int64_t, T>::value ||
  std::is_same<int32_t, T>::value ||
  std::is_same<int16_t, T>::value ||
  std::is_same<bool,    T>::value ||
  std::is_same<std::atomic<int64_t>, T>::value ||
  std::is_same<std::atomic<int32_t>, T>::value ||
  std::is_same<std::atomic<int16_t>, T>::value ||
  std::is_same<std::atomic<bool>,    T>::value>::type;

public:
	c_autoset(T& variable, const T& value) :
		_variable(variable), _value(value) {
	}

	~c_autoset() {
		_variable = _value;
	}

private:
	T& _variable;
	const T _value;

private:
	c_autoset(const c_autoset&) = delete;
	c_autoset& operator=(const c_autoset&) = delete;
};

}

#endif /* _CC_AUTOSET_H */
