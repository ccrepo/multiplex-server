//
// cc_strings.h
//

#ifndef _CC_STRINGS_H
#define _CC_STRINGS_H

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace cc {

class c_strings final {

public:
	inline static const std::string& endl() {
		static const std::string l = []() -> std::string {
			std::ostringstream os;
			os << std::endl;
			return os.str();
		}();

		static const std::string el(l);
		return el;
	}

	inline static std::string ltrim(const std::string &original) {
		std::string copy(original);

		copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), [](char c) {
			return !std::isspace(c);
		}));

		return copy;
	}

	inline static std::string rtrim(const std::string &original) {
		std::string copy(original);

		copy.erase(std::find_if(copy.rbegin(), copy.rend(), [](char c) {
			return !std::isspace(c);
		}).base(), copy.end());

		return copy;
	}

	inline static std::string trim(const std::string &s) {
		return c_strings::ltrim(c_strings::rtrim(s));
	}

	inline static std::string trim(const char* &s) {
		return c_strings::ltrim(c_strings::rtrim(std::string(s)));
	}

	inline static std::string to_lower(const std::string &original) {
		std::string copy(original);
		std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
		return copy;
	}

  inline static std::string to_upper(const std::string &original) {
		std::string copy(original);
		std::transform(copy.begin(), copy.end(), copy.begin(), ::toupper);
		return copy;
	}

  template<typename T>
  inline static std::string to_string(T value) {

  	using value_type [[maybe_unused]] = typename std::enable_if<
    std::is_same<int64_t, T>::value ||
    std::is_same<int32_t, T>::value ||
    std::is_same<int16_t, T>::value ||
  	std::is_same<float,   T>::value ||
  	std::is_same<double,  T>::value ||
  	std::is_same<char,    T>::value>::type;

  	std::ostringstream os;
  	os << value;
  	return os.str();
  }

  template<typename T>
  inline static std::string to_string(const std::vector<T>& values) {
  	std::ostringstream os;
  	bool first(true);

    for (const auto& value : values) {
    	if (first) {
  			first = false;
    	} else {
    		os << ",";
    	}

			os << "\"" << value << "\"";
		}

    return os.str();
  }

  static int32_t extract_leading_number_length_n(const std::string& buffer, int32_t n);
  
private:
	c_strings() = delete;
	c_strings(c_strings&) = delete;
	c_strings& operator=(c_strings&) = delete;
	~c_strings() = delete;
};

}

#endif /* _CC_STRINGS_H */

