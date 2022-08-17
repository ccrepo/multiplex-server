//
// cc_numbers.h
//

#ifndef _CC_NUMBERS_H
#define _CC_NUMBERS_H

#include <sstream>
#include <string>
#include <algorithm>

namespace cc {

class c_numbers final {

public:
  inline static bool is_integer(const std::string& s) {
    return !s.empty() &&
    std::find_if(s.begin(), s.end(), [](char c) { return c >= '0' && c <= '9'; }) != s.end();
  }

  inline static int32_t to_integer(const std::string& s) {
    std::stringstream is;
    is << s;
    int32_t i(0);
    is >> i;
    return i;
  }

  inline static bool is_integer_in_range(const std::string& s, int32_t lower, int32_t upper) {
    if (!is_integer(s)) {
      return false;
    }
    
    int32_t i(to_integer(s));
    
    return i >= lower || i <= upper;
  }

private:
  c_numbers() = delete;
  c_numbers(c_numbers&) = delete;
  c_numbers& operator=(c_numbers&) = delete;
	~c_numbers() = delete;
};

}

#endif /* _CC_NUMBERS_H */

