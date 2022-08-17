//
// cc_log.h
//

#ifndef _CC_LOG_H
#define _CC_LOG_H

#include <iostream>
#include <sstream>
#include <atomic>
#include <string>
#include <iomanip>
#include <chrono>
using namespace std::chrono_literals;
#include <cstring>
#include <thread>

#include "cc_strings.h"
#include "cc_file.h"
#include "cc_files.h"
#include "cc_config.h"
#include "cc_system.h"
#include "cc_attributes.h"

namespace cc {
namespace log {

class c_fatal;
class c_error;
class c_warn;
class c_info;
class c_debug;
class c_trace;

}

template<typename T>
class c_loggerT;

class c_logger_base : private c_attributes_exit {

public:
  static constexpr const char *_LOGLEVEL_TRACE { "trace" };
  static constexpr const char *_LOGLEVEL_DEBUG { "debug" };
  static constexpr const char *_LOGLEVEL_INFO  { "info" };
  static constexpr const char *_LOGLEVEL_WARN  { "warn" };
  static constexpr const char *_LOGLEVEL_ERROR { "error" };
  static constexpr const char *_LOGLEVEL_FATAL { "fatal" };

	static bool is_fatal_level_called() {
    return c_attributes_exit::is_attribute_exit_global();
	}

  static void set_logfile() {
    
  }

	inline static bool all_levels_off() {
    if (c_config::valid()) {
       static const bool off((static_cast<int32_t>(c_config::singleton()->diagnostics_log_level()  == _LOGLEVEL_TRACE) +
                             static_cast<int32_t>(c_config::singleton()->diagnostics_log_level()  == _LOGLEVEL_DEBUG) +
                             static_cast<int32_t>(c_config::singleton()->diagnostics_log_level()  == _LOGLEVEL_WARN) +
                             static_cast<int32_t>(c_config::singleton()->diagnostics_log_level()  == _LOGLEVEL_TRACE) +
                             static_cast<int32_t>(c_config::singleton()->diagnostics_log_level()  == _LOGLEVEL_ERROR) +
                             static_cast<int32_t>(c_config::singleton()->diagnostics_log_level()  == _LOGLEVEL_FATAL)) == 0);
       return off;
    }
    return false;
	}

	inline static std::string ts() {
		auto now(std::chrono::system_clock::now());
		auto ts(std::chrono::system_clock::to_time_t(now));

		std::ostringstream os;
		os << std::put_time(std::localtime(&ts), "%Y%m%d %H:%M:%S");
		//os << std::put_time(std::localtime(&ts), "%H:%M:%S");
		return os.str();
	}

protected:
	static int32_t thread_id() {
		static std::atomic<int32_t> count { 1 };
		static thread_local const int32_t id(count.fetch_add(1));
		return id;
	}

	static const std::string& thread_name() {
		static const std::string prefix("thread-");
		static thread_local const int id { c_logger_base::thread_id() };
		static thread_local const std::string name(prefix + std::to_string(id));
		return name;
	}

protected:
	c_logger_base() : c_attributes_exit() {
	}
  
	~c_logger_base() {
	}

private:
	c_logger_base& operator=(const c_logger_base&) = delete;
	c_logger_base(const c_logger_base&) = delete;
};

template<typename T>
class c_logger_implT final : public c_logger_base {

	using value_type [[maybe_unused]] = typename std::enable_if<
	std::is_same<cc::log::c_fatal, T>::value ||
	std::is_same<cc::log::c_error, T>::value ||
	std::is_same<cc::log::c_warn,  T>::value ||
	std::is_same<cc::log::c_info,  T>::value ||
	std::is_same<cc::log::c_debug, T>::value ||
	std::is_same<cc::log::c_trace, T>::value>::type;

public:
	~c_logger_implT() {
		this->flush();
	}

	template<typename U>
	c_logger_implT<T>& operator<<(const U &u) {
		static bool isLevel(c_logger_implT<T>::is_level_on());

		if (isLevel) {
			_os << u;
		}

		if (std::is_same<T, cc::log::c_fatal>::value) {
      cc::c_attributes_exit::set_global_exit_flag();
		}

		return (*this);
	}

	c_logger_implT<T>& operator<<(c_logger_implT<T>& (*fp)(c_logger_implT<T> &logger)) {
		return fp(*this);
	}

	void flush() {
    if (_os.str().length() > 0) {
			std::ostringstream os;
			
      os << this->ts()
      << " ["
      << thread_name()
      << "|"
      << prefix()
      << "] "
      // << "{"
      // << std::this_thread::get_id()
      // << "} "
      << _os.str();

      if (c_config::valid()) {
        if (c_config::singleton()->diagnostics_log_console()) {
          std::cout << os.str();
        }
        
        if (c_config::singleton()->diagnostics_log_output()) {
          static std::shared_ptr<cc::c_ofile> logfile(c_config::singleton()->logfile());
          
          if (logfile.get()) {
            logfile->write(os.str());
          }
        }
      }

      clear();
		}
	}

	void clear() {
		_os.str("");
	}

	static bool is_level_on();
	const char* prefix() const;

private:
	c_logger_implT() {
	}

private:
	c_logger_implT& operator=(const c_logger_implT&) = delete;
	c_logger_implT(const c_logger_implT&) = delete;

private:
	friend class c_loggerT<T> ;

private:
	std::ostringstream _os;
};

template<>
inline const char* c_logger_implT<log::c_fatal>::prefix() const {
	return _LOGLEVEL_FATAL;
}

template<>
inline const char* c_logger_implT<log::c_error>::prefix() const {
	return _LOGLEVEL_ERROR;
}

template<>
inline const char* c_logger_implT<log::c_warn>::prefix() const {
	return _LOGLEVEL_WARN;
}

template<>
inline const char* c_logger_implT<log::c_info>::prefix() const {
	return _LOGLEVEL_INFO;
}

template<>
inline const char* c_logger_implT<log::c_debug>::prefix() const {
	return _LOGLEVEL_DEBUG;
}

template<>
inline const char* c_logger_implT<log::c_trace>::prefix() const {
	return _LOGLEVEL_TRACE;
}

template<typename T> // checked elsewhere
inline const char* c_logger_implT<T>::prefix() const {
	return "";
}

template<>
inline bool c_logger_implT<cc::log::c_trace>::is_level_on() {
  static thread_local bool guard_off(false);

  if (guard_off || (c_config::valid() &&
      (cc::c_config::singleton()->diagnostics_log_console() ||
       cc::c_config::singleton()->diagnostics_log_output()))) {

    static const bool on(((guard_off = true)) &&
                         (c_config::singleton()->diagnostics_log_level() == _LOGLEVEL_TRACE));

     return on;
  }
  
  return false;
}

template<>
inline bool c_logger_implT<cc::log::c_debug>::is_level_on() {
  static thread_local bool guard_off(false);

  if (guard_off || (c_config::valid() &&
      (cc::c_config::singleton()->diagnostics_log_console() ||
       cc::c_config::singleton()->diagnostics_log_output()))) {
    
     static const bool on(((guard_off = true)) &&
                          (c_config::singleton()->diagnostics_log_level() == _LOGLEVEL_DEBUG ||
                           cc::c_logger_implT<cc::log::c_trace>::is_level_on()));

     return on;
  }
  return false;
}

template<>
inline bool c_logger_implT<cc::log::c_info>::is_level_on() {
  static thread_local bool guard_off(false);

  if (guard_off || (c_config::valid() &&
      (cc::c_config::singleton()->diagnostics_log_console() ||
       cc::c_config::singleton()->diagnostics_log_output()))) {
    
     static const bool on(((guard_off = true)) &&
         (c_logger_base::all_levels_off() ?
             true :
             (c_config::singleton()->diagnostics_log_level() == _LOGLEVEL_INFO ||
              cc::c_logger_implT<cc::log::c_debug>::is_level_on())));

     return on;
  }
  
  return false;
}

template<>
inline bool c_logger_implT<cc::log::c_warn>::is_level_on() {
  static thread_local bool guard_off(false);

  if (guard_off || (c_config::valid() &&
      (cc::c_config::singleton()->diagnostics_log_console() ||
       cc::c_config::singleton()->diagnostics_log_output()))) {
    
     static const bool on(((guard_off = true)) &&
         (c_logger_base::all_levels_off() ?
             true :
             (c_config::singleton()->diagnostics_log_level() == _LOGLEVEL_WARN ||
              cc::c_logger_implT<cc::log::c_info>::is_level_on())));

     return on;
  }
  
  return false;
}

template<>
inline bool c_logger_implT<cc::log::c_error>::is_level_on() {
  static thread_local bool guard_off(false);
  
  if (guard_off || (c_config::valid() &&
      (cc::c_config::singleton()->diagnostics_log_console() ||
       cc::c_config::singleton()->diagnostics_log_output()))) {
    
     static const bool on(((guard_off = true)) &&
         (c_logger_base::all_levels_off() ?
             true :
             (c_config::singleton()->diagnostics_log_level() == _LOGLEVEL_ERROR ||
              cc::c_logger_implT<cc::log::c_warn>::is_level_on())));

     return on;
  }
  
  return false;
}

template<>
inline bool c_logger_implT<cc::log::c_fatal>::is_level_on() {
  static thread_local bool guard_off(false);
  
  if (guard_off || (c_config::valid() &&
      (cc::c_config::singleton()->diagnostics_log_console() ||
       cc::c_config::singleton()->diagnostics_log_output()))) {
    
     static const bool on(((guard_off = true)) &&
         (c_logger_base::all_levels_off() ?
             true :
             (c_config::singleton()->diagnostics_log_level() == _LOGLEVEL_FATAL ||
              cc::c_logger_implT<cc::log::c_error>::is_level_on())));

     return on;
  }
  
  return false;

}

template<typename T>
class c_loggerT final {

	using value_type [[maybe_unused]] = typename std::enable_if<
	std::is_same<cc::log::c_fatal, T>::value ||
	std::is_same<cc::log::c_error, T>::value ||
	std::is_same<cc::log::c_warn,  T>::value ||
	std::is_same<cc::log::c_info,  T>::value ||
	std::is_same<cc::log::c_debug, T>::value ||
	std::is_same<cc::log::c_trace, T>::value>::type;

public:
	c_loggerT() :
			_logger() {
	}

	~c_loggerT() {
	}

	template<typename U>
	c_logger_implT<T>& operator<<(const U &data) {
		_logger << data;
		return _logger;
	}

private:
	c_logger_implT<T> _logger;

private:
	c_loggerT(const c_loggerT&) = delete;
	c_loggerT& operator=(const c_loggerT&) = delete;
};

template<typename T>
c_logger_implT<T>& endl(c_logger_implT<T> &logger) {

	using value_type [[maybe_unused]] = typename std::enable_if<
	std::is_same<cc::log::c_fatal, T>::value ||
	std::is_same<cc::log::c_error, T>::value ||
	std::is_same<cc::log::c_warn,  T>::value ||
	std::is_same<cc::log::c_info,  T>::value ||
	std::is_same<cc::log::c_debug, T>::value ||
	std::is_same<cc::log::c_trace, T>::value>::type;

	static const std::string el(c_strings::endl());
	logger << el;
	return logger;
}

inline std::ostringstream& operator<<(std::ostringstream &os,
		const std::ostringstream &s) {
	os << s.str();
	return os;
}

template<typename T, typename U>
void write(std::ostringstream &os, const U &u) {
	os << u;
}

template<typename T, typename U, typename ... Args>
void write(std::ostringstream &os, const U &u, const Args &... args) {
	os << u;
	cc::write<T, Args...>(os, args...);
}

template<typename T, typename ... Args>
void write(cc::c_loggerT<T> &&logger, const char *file, uint32_t lineno,
		const char *function, const Args &... args) {
	std::ostringstream os;
	os << file << ":" << lineno << " (" << function << ") ";
	cc::write<T, Args...>(os, args...);
	logger << os.str() << cc::endl;
}

#define LOG_TRACE(...) {                                                   \
if (cc::c_logger_implT<cc::log::c_trace>::is_level_on())                   \
{                                                                          \
constexpr const char* basename(cc::c_files::basename_slash(__FILE__));     \
cc::write<cc::log::c_trace>                                                \
(cc::c_loggerT<cc::log::c_trace>(),                                        \
basename,                                                                  \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOG_DEBUG(...) {                                                   \
if (cc::c_logger_implT<cc::log::c_debug>::is_level_on())                   \
{                                                                          \
constexpr const char* basename(cc::c_files::basename_slash(__FILE__));     \
cc::write<cc::log::c_debug>                                                \
(cc::c_loggerT<cc::log::c_debug>(),                                        \
basename,                                                                  \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOG_INFO(...) {                                                    \
if (cc::c_logger_implT<cc::log::c_info>::is_level_on())                    \
{                                                                          \
constexpr const char* basename(cc::c_files::basename_slash(__FILE__));     \
cc::write<cc::log::c_info>                                                 \
(cc::c_loggerT<cc::log::c_info>(),                                         \
basename,                                                                  \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOG_WARN(...) {                                                    \
if (cc::c_logger_implT<cc::log::c_warn>::is_level_on())                    \
{                                                                          \
constexpr const char* basename(cc::c_files::basename_slash(__FILE__));     \
cc::write<cc::log::c_warn>                                                 \
(cc::c_loggerT<cc::log::c_warn>(),                                         \
basename,                                                                  \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOG_ERROR(...) {                                                   \
if (cc::c_logger_implT<cc::log::c_error>::is_level_on())                   \
{                                                                          \
constexpr const char* basename(cc::c_files::basename_slash(__FILE__));     \
cc::write<cc::log::c_error>                                                \
(cc::c_loggerT<cc::log::c_error>(),                                        \
basename,                                                                  \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOG_FATAL(...) {                                                   \
if (cc::c_logger_implT<cc::log::c_fatal>::is_level_on())                   \
{                                                                          \
constexpr const char* basename(cc::c_files::basename_slash(__FILE__));     \
cc::write<cc::log::c_fatal>                                                \
(cc::c_loggerT<cc::log::c_fatal>(),                                        \
basename,                                                                  \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
cc::write<cc::log::c_fatal>                                                \
(cc::c_loggerT<cc::log::c_fatal>(),                                        \
basename,                                                                  \
__LINE__, __FUNCTION__, "FATAL message will signal exit");                 \
}                                                                          \
}

}

#endif /* _CC_LOG_H */

