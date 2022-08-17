//
// cc_system.cpp
//

#include "cc_system.h"
#include "cc_strings.h"
#include "cc_log.h"

namespace cc {

inline bool c_system::check_environment(const std::string &name,
    const std::string &value) {
  return c_system::getenv(name) == value;
}

inline std::string c_system::getenv(const std::string &name) {
  char *value { std::getenv(name.c_str()) };
  if (value == nullptr || strlen(value) == 0) {
    return "";
  }
  return c_strings::to_lower(c_strings::trim(value));
}

}

