//
// cc_strings.cpp
//

#include "cc_strings.h"

namespace cc {

int32_t c_strings::extract_leading_number_length_n(const std::string& buffer, int32_t n) {
  
  if (buffer.length() < n) {
    return -1;
  }
  
  auto iter { std::find_if_not(buffer.begin(), buffer.begin() + n, [](char c) { return c >= '0' && c <= '9'; } ) };
  
  if ((iter > (buffer.begin() + n)) ||
      (iter - buffer.begin()) != n) {
    return -1;
  }
  
  int result(std::stoi(buffer.substr(0, n)));
  
  if (result < n) {
    return -1;
  }
  
  return result;
}

}

