//
// cc_system.h
//

#include <string>

#include "cc_macros.h"
#include "cc_build.h"

#ifndef _CC_SYSTEM_H
#define _CC_SYSTEM_H

#ifdef _WIN64
static_assert(0, "platform NOT suppprted");
#elif _WIN32
static_assert(0, "platform NOT suppprted");
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE && TARGET_OS_SIMULATOR
static_assert(0, "platform NOT suppprted");
#elif TARGET_OS_IPHONE && TARGET_OS_MACCATALYST
static_assert(0, "platform NOT suppprted");
#elif TARGET_OS_IPHONE
static_assert(0, "platform NOT suppprted");
#else
#define CC_COMPILE_MACOS
#endif
#elif __linux
#define CC_COMPILE_LINUX
#elif __unix
static_assert(0, "platform NOT suppprted");
#elif __posix
#else
static_assert(0, "platform NOT suppprted");
#endif

namespace cc {

class c_system final {

public:
  inline static bool check_environment(const std::string &name,
                                       const std::string &value);

  inline static std::string getenv(const std::string &name);
  
private:
  c_system() = delete;
  c_system(c_system&) = delete;
  c_system& operator=(c_system&) = delete;
  ~c_system() = delete;
};

}

#endif /* _CC_SYSTEM_H */

