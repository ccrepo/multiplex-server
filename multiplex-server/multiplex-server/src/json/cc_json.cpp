//
// cc_json.cpp
//

#include "cc_json.h"
#include "cc_strings.h"

namespace cc {

bool c_json::extract_configuration_from_json(const nlohmann::json& j_parent,
                                             OUT std::vector<std::string>& j_path,
                                             OUT std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE,
                                             void*>>& extracted_configuration) {
  
  bool result(true);
  
  for (auto j_child = j_parent.items().begin(); j_child != j_parent.items().end(); ++j_child) {
    
    std::vector<std::string> child_j_path(j_path);
    child_j_path.push_back(j_child.key());
    
    if(j_child.value().is_object()) {
      result = extract_configuration_from_json(j_child.value(), child_j_path, extracted_configuration) && result;
      
    } else if (extracted_configuration.find(child_j_path) != extracted_configuration.end()) {
      std::cerr << "duplicate json element " << child_j_path.size() << " " << j_child.key() << " at " << j_parent << std::endl;
      result = false;
      
    } else if(j_child.value().is_string()) {
      extracted_configuration[child_j_path] =
      std::make_tuple(c_config_element::CONFIG_ELEMENTYPE_STRING, reinterpret_cast<void*>(new std::string(j_child.value())));
      
    } else if(j_child.value().is_boolean()) {
      extracted_configuration[child_j_path] =
      std::make_tuple(c_config_element::CONFIG_ELEMENTYPE_BOOL,reinterpret_cast<void*>(new bool(j_child.value())));
      
    } else if(j_child.value().is_number_integer()) {
      extracted_configuration[child_j_path] =
      std::make_tuple(c_config_element::CONFIG_ELEMENTYPE_LONG, reinterpret_cast<void*>(new int64_t(j_child.value())));
      
    } else if(j_child.value().is_number_float()) {
      extracted_configuration[child_j_path] =
      std::make_tuple(c_config_element::CONFIG_ELEMENTYPE_DOUBLE, reinterpret_cast<void*>(new double(j_child.value())));
      
    } else {
      std::cerr << "invalid json element " << j_child.key() << " " << j_child.value() << " at " << j_parent << " - type not handled" << std::endl;
      result = false;
    }
  }
  
  return result;
}

bool c_json::is_configured_and_valid(const std::vector<std::unique_ptr<c_config_element>> &configured_config_elements,
                                     std::vector<std::string> json_path,
                                     c_config_element::CONFIG_ELEMENTYPE element_type, void* value) {

	auto iter { std::find_if(configured_config_elements.begin(), configured_config_elements.end(),
			[&json_path, element_type](auto &config_element) {
				return config_element->json_path() == json_path && config_element->element_type() == element_type;
			}) };

	if (iter == configured_config_elements.end()) {
		return false;
	}

	if (!iter->get()->is_valid_value(c_strings::to_string(iter->get()->json_path()), value)) {
		return false;
	}

	return true;
}

}
