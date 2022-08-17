//
// cc_config_element.cpp
//

#include "cc_config_element.h"
#include "cc_strings.h"
#include "cc_collections.h"
#include "cc_bools.h"
#include "cc_tuples.h"

namespace cc {

// string
c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::vector<std::string>> &constraints,
                                   const std::string &defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_STRING),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description),
    _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new std::string(defauit_value)),
    _default_value_as_string(defauit_value),
    _value(nullptr),
    _constraints(constraints.get() == nullptr ? nullptr : new std::vector<std::string>(*constraints.get())) {
}

c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::vector<std::string>> &constraints,
                                   const char *defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_STRING),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description),
    _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new std::string(defauit_value)),
    _default_value_as_string(defauit_value),
    _value(nullptr),
    _constraints(constraints.get() == nullptr ? nullptr : new std::vector<std::string>(*constraints.get())) {
}

// int64_t
c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::tuple<int64_t, int64_t>> &constraints,
                                   int64_t defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_LONG),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description),
    _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new int64_t(defauit_value)),
    _default_value_as_string(c_strings::to_string(defauit_value)),
    _value(nullptr),
    _constraints(constraints.get() == nullptr ? nullptr : new std::tuple<int64_t, int64_t>(std::get<0>(*constraints.get()), std::get<1>(*constraints.get()))) {
}

c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::tuple<int64_t, int64_t>> &constraints,
                                   int32_t defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_LONG),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description),
    _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new int64_t(defauit_value)),
    _default_value_as_string(c_strings::to_string(defauit_value)), _value(nullptr),
    _constraints(constraints.get() == nullptr ? nullptr : new std::tuple<int64_t, int64_t>(std::get<0>(*constraints.get()), std::get<1>(*constraints.get()))) {
}

c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::tuple<int64_t, int64_t>> &constraints,
                                   int16_t defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_LONG),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description),
    _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new int64_t(defauit_value)),
    _default_value_as_string(c_strings::to_string(defauit_value)),
    _value(nullptr), _constraints(
				constraints.get() == nullptr ?
						nullptr : new std::tuple<int64_t, int64_t>(std::get<0>(*constraints.get()), std::get<1>(*constraints.get()))) {
}

// double
c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::tuple<double, double>> &constraints,
                                   float defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_DOUBLE),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description), _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new double(defauit_value)),
    _default_value_as_string(c_strings::to_string(defauit_value)),
    _value(nullptr), _constraints(
				constraints.get() == nullptr ?
						nullptr : new std::tuple<double, double>(std::get<0>(*constraints.get()), std::get<1>(*constraints.get()))) {
}

c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::tuple<double, double>> &constraints,
                                   double defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_DOUBLE),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description), _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new double(defauit_value)),
    _default_value_as_string(c_strings::to_string(defauit_value)),
    _value(nullptr), _constraints(
				constraints.get() == nullptr ?
						nullptr : new std::tuple<double, double>(std::get<0>(*constraints.get()), std::get<1>(*constraints.get()))) {
}

// bool
c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   bool defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_BOOL),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description),
    _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new bool(defauit_value)),
    _default_value_as_string(c_bools::to_string(defauit_value)),
    _value(nullptr),
    _constraints(nullptr) {
}

// char
c_config_element::c_config_element(const std::string &description,
                                   const std::vector<std::string> &json_path,
                                   const std::unique_ptr<const std::vector<char>> &constraints,
                                   char defauit_value,
                                   bool mandatory) :
		_element_type(CONFIG_ELEMENTYPE_CHAR),
    _name(c_collections::collection_to_string(json_path.begin(), json_path.end(), '|')),
    _description(description),
    _json_path(json_path),
    _mandatory(mandatory),
    _default_value(new char(defauit_value)),
    _default_value_as_string(c_strings::to_string(defauit_value)),
    _value(nullptr),
    _constraints(constraints.get() == nullptr ? nullptr : new std::vector<char>(*constraints.get())) {
}

//

c_config_element::~c_config_element() {

	try {
		switch (_element_type) {

		case CONFIG_ELEMENTYPE_STRING: {
			delete reinterpret_cast<const std::string*>(_default_value);
			delete reinterpret_cast<const std::string*>(_value);
			delete reinterpret_cast<std::vector<std::string>*>(_constraints);
			break;
		}

		case CONFIG_ELEMENTYPE_LONG: {
			delete reinterpret_cast<const int64_t*>(_default_value);
			delete reinterpret_cast<const int64_t*>(_value);
			delete reinterpret_cast<std::tuple<int64_t, int64_t>*>(_constraints);
			break;
		}

		case CONFIG_ELEMENTYPE_DOUBLE: {
			delete reinterpret_cast<const double*>(_default_value);
			delete reinterpret_cast<const double*>(_value);
			delete reinterpret_cast<std::tuple<double, double>*>(_constraints);
			break;
		}

		case CONFIG_ELEMENTYPE_BOOL: {
			delete reinterpret_cast<const bool*>(_default_value);
			delete reinterpret_cast<const bool*>(_value);
			break;
		}

		case CONFIG_ELEMENTYPE_CHAR: {
			delete reinterpret_cast<const char*>(_default_value);
			delete reinterpret_cast<const char*>(_value);
			delete reinterpret_cast<std::vector<char>*>(_constraints);
			break;
		}

		}

	} catch (...) {
		std::cerr << "unknown exception in ~c_config_element" << std::endl;
	}
}

const char* c_config_element::get_element_type_as_string() const {

	switch (_element_type) {

	case CONFIG_ELEMENTYPE_STRING: {
		return "string";
	}

	case CONFIG_ELEMENTYPE_LONG: {
		return "integer";
	}

	case CONFIG_ELEMENTYPE_DOUBLE: {
		return "float";
	}

	case CONFIG_ELEMENTYPE_BOOL: {
		return "bool";
	}

	case CONFIG_ELEMENTYPE_CHAR: {
		return "char";
	}

	}

	return "unknown";
}

std::string c_config_element::get_constraints_as_string() const {

	if (_constraints == nullptr) {
		return "";
	}

	switch (_element_type) {

	case CONFIG_ELEMENTYPE_BOOL: {
		return "";
	}

	case CONFIG_ELEMENTYPE_STRING: {
		std::vector<std::string> *constraints(reinterpret_cast<std::vector<std::string>*>(_constraints));

		if (!constraints->empty()) {
			std::ostringstream os;
			os << "options { " << c_collections::collection_to_string(constraints->begin(), constraints->end()) << " }";
			return os.str();
		}

		return std::string();
	}

	case CONFIG_ELEMENTYPE_LONG: {
		std::tuple<int64_t, int64_t> *constraints(reinterpret_cast<std::tuple<int64_t, int64_t>*>(_constraints));
		return c_tuples::tuple_to_string_range(*constraints);
	}

	case CONFIG_ELEMENTYPE_DOUBLE: {
		std::tuple<double, double> *constraints(reinterpret_cast<std::tuple<double, double>*>(_constraints));
		return c_tuples::tuple_to_string_range(*constraints);
	}

	case CONFIG_ELEMENTYPE_CHAR: {
		return "";
	}

	}
}

bool c_config_element::is_valid_value(const std::string &json_path, void *value) const {

	if (value == nullptr) {
		return false;
	}

	if (_constraints == nullptr) {
		return true;
	}

	switch (_element_type) {

	case CONFIG_ELEMENTYPE_BOOL: {
		return true;
	}

	case CONFIG_ELEMENTYPE_STRING: {
		std::string *converted(reinterpret_cast<std::string*>(value));
		std::vector<std::string> *constraints(reinterpret_cast<std::vector<std::string>*>(_constraints));

		if (constraints->empty() || std::find(constraints->begin(), constraints->end(), *converted) != constraints->end()) {
			return true;
		}

		std::cerr << "value " << *converted << " breaks constraint at path " << json_path << std::endl;
		break;
	}

	case CONFIG_ELEMENTYPE_LONG: {
    int64_t *converted(reinterpret_cast<int64_t*>(value));
		std::tuple<int64_t, int64_t> *constraints(reinterpret_cast<std::tuple<int64_t, int64_t>*>(_constraints));

		if (*converted >= std::get<0>(*constraints) && *converted <= std::get<1>(*constraints)) {
			return true;
		}

		std::cerr << "value " << *converted << " breaks constraint at path " << json_path << std::endl;
		break;
	}

	case CONFIG_ELEMENTYPE_DOUBLE: {
		double *converted(reinterpret_cast<double*>(value));
		std::tuple<double, double> *constraints(reinterpret_cast<std::tuple<double, double>*>(_constraints));

		if (*converted >= std::get<0>(*constraints) && *converted <= std::get<1>(*constraints)) {
			return true;
		}

		std::cerr << "value " << *converted << " breaks constraint at path " << json_path << std::endl;
		break;
	}

	case CONFIG_ELEMENTYPE_CHAR: {
		char *converted(reinterpret_cast<char*>(value));
		std::vector<char> *constraints(reinterpret_cast<std::vector<char>*>(_constraints));

		if (constraints->empty() || std::find(constraints->begin(), constraints->end(), *converted) != constraints->end()) {
			return true;
		}

		std::cerr << "value " << *converted << " breaks constraint at path " << json_path << std::endl;
		break;
	}

	}

	return false;
}

}
