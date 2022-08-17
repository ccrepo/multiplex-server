//
// cc_json.h
//

#ifndef _CC_JSON_H
#define _CC_JSON_H

#include <tuple>
#include <string>

#include "json.h"
#include "cc_config_element.h"
#include "cc_macros.h"

namespace cc {

class c_json final {

public:
  static bool extract_configuration_from_json(const nlohmann::json& j_parent,
                                              OUT std::vector<std::string>& j_path,
                                              OUT std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE,
                                              void*>>& extracted_configuration);

	static bool is_configured_and_valid(const std::vector<std::unique_ptr<c_config_element>> &configured_config_elements, // TODO - tidy
                                      std::vector<std::string> json_path,
                                      c_config_element::CONFIG_ELEMENTYPE element_type, void* value);

private:
	c_json() = delete;
	c_json(c_json&) = delete;
	c_json& operator=(c_json&) = delete;
	~c_json() = delete;
};

}

#endif /* _CC_JSON_H */
