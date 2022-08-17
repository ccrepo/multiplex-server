//
// cc_config_element.h
//

#ifndef _CC_CONFIG_ELEMENT_H
#define _CC_CONFIG_ELEMENT_H

#include <memory>
#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <vector>
#include <tuple>

namespace cc {

class c_config_element final {

public:
	enum CONFIG_ELEMENTYPE {
		CONFIG_ELEMENTYPE_STRING, CONFIG_ELEMENTYPE_LONG, CONFIG_ELEMENTYPE_DOUBLE, CONFIG_ELEMENTYPE_BOOL, CONFIG_ELEMENTYPE_CHAR
	};

private:

public:
  // string
	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::vector<std::string>> &constraints,
                   const std::string &defauit_value,
                   bool mandatory);

	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::vector<std::string>> &constraints,
                   const char *defauit_value,
                   bool mandatory);

  // int
	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::tuple<int64_t, int64_t>> &constraints,
                   int64_t defauit_value,
                   bool mandatory);

	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::tuple<int64_t, int64_t>> &constraints,
                   int32_t defauit_value,
                   bool mandatory);

	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::tuple<int64_t, int64_t>> &constraints,
                   int16_t defauit_value,
                   bool mandatory);

  // double
	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::tuple<double, double>> &constraints,
                   float defauit_value,
                   bool mandatory);

	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::tuple<double, double>> &constraints,
                   double defauit_value,
                   bool mandatory);

  // char
	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   bool defauit_value,
                   bool mandatory);

  // bool
	c_config_element(const std::string &description,
                   const std::vector<std::string> &json_path,
                   const std::unique_ptr<const std::vector<char>> &constraintss,
                   char defauit_value,
                   bool mandatory);

public:
	~c_config_element();

	const char* get_element_type_as_string() const;

	std::string get_constraints_as_string() const;

	bool is_valid_value(const std::string& json_path, void* value) const;

	inline CONFIG_ELEMENTYPE element_type() const {
		return _element_type;
	}

	inline const std::string& name() const {
		return _name;
	}

	inline const std::string& description() const {
		return _description;
	}

	inline const std::vector<std::string>& json_path() const {
		return _json_path;
	}

	inline bool mandatory() const {
		return _mandatory;
	}

	inline const void* default_value() const {
		return _default_value;
	}

	inline const void* value() const {
		return _value;
	}

	inline const void* constraints() const {
		return _constraints;
	}

	const std::string& default_value_as_string() const {
		return _default_value_as_string;
	}

private:
	const CONFIG_ELEMENTYPE _element_type;
	const std::string _element_type_as_string;
	const std::string _name;
	const std::string _description;
	const std::vector<std::string> _json_path;
	const bool _mandatory; // TODO - start using this
	const void *_default_value;
	const std::string _default_value_as_string;
	void *_value;
	void *_constraints;

private:
	inline static std::atomic<size_t> _query_id_counter { 0 };

private:
	c_config_element(const c_config_element&) = delete;
	c_config_element& operator=(const c_config_element&) = delete;
};

}

#endif /* _CC_CONFIG_ELEMENT_H */
