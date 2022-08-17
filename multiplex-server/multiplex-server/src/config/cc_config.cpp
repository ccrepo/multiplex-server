//
// cc_config.cpp
//

#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <string>
#include <map>

#include "cc_config.h"
#include "cc_strings.h"
#include "cc_file.h"
#include "cc_json.h"
#include "cc_bools.h"
#include "cc_collections.h"

namespace cc {

std::shared_ptr<c_config> c_config::_config;

std::atomic<bool> c_config::_valid(false);
bool c_config::_help_requested(false);
std::unique_ptr<nlohmann::json> c_config::_json(new nlohmann::json());
std::vector<std::unique_ptr<c_config_element>> c_config::_config_elements;

bool c_config::init(int32_t argc, const char *argv[]) {
	static const bool loaded(c_config::configure() && c_config::load(argc, argv));

	return loaded;
}

bool c_config::configure() {

	static const std::unique_ptr<const std::vector<std::string>> string_no_constraints(new std::vector<std::string>());

  std::vector<c_config_element*> config_elements {
    
    new c_config_element("type of process to run",
                         _process_type_path,
                         std::unique_ptr<const std::vector<std::string>>(new std::vector<std::string> { "server", "client" }),
                         "",
                         true),
    
    new c_config_element("admin account username",
                         _admin_username_path,
                         string_no_constraints,
                         "admin",
                         true),
    
    new c_config_element("admin account password",
                         _admin_password_path,
                         string_no_constraints,
                         "password",
                         true),
    
    new c_config_element("capacity datamodel pool size",
                         _capacity_datamodel_pool_size_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 250)),
                         2,
                         true),

    new c_config_element("capacity listener pool size",
                         _capacity_listener_pool_size_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 1)),
                         1,
                         true),

    new c_config_element("capacity reader pool size",
                         _capacity_reader_pool_size_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 250)),
                         2,
                         true),

    new c_config_element("capacity writer pool size",
                         _capacity_writer_pool_size_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 250)),
                         2,
                         true),

    new c_config_element("internal wait service exit",
                         _internal_wait_service_exit_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(25, 100000)),
                         1000,
                         true),

    new c_config_element("internal test speed slow",
                         _internal_test_speed_slow_path,
                         true,
                         true),
    
    new c_config_element("diagnostics log to console",
                         _diagnostics_log_console_path,
                         true,
                         true),

    new c_config_element("diagnostics log to dump connections",
                         _diagnostics_log_dump_connections_path,
                         true,
                         true),

    new c_config_element("diagnostics log output",
                         _diagnostics_log_output_path,
                         true,
                         true),

    new c_config_element("diagnostics log truncate",
                         _diagnostics_log_truncate_path,
                         true,
                         true),

    new c_config_element("diagnostics log filename",
                         _diagnostics_log_filename_path,
                         string_no_constraints,
                         "/tmp/battleship-log.txt",
                         true),
    
    new c_config_element("diagnostics log level",
                         _diagnostics_log_level_path,
                         std::unique_ptr<const std::vector<std::string>>(new std::vector<std::string> { "dev", "trace", "debug", "info", "warn", "error", "fatal" }),
                         "info",
                         true),
    
    new c_config_element("network connection buffer size",
                         _network_connection_buffer_size_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(512, 102400)),
                         1024,
                         true),

    new c_config_element("network connection bind ip",
                         _network_connection_bind_ip_path,
                         string_no_constraints,
                         "",
                         true),

    new c_config_element("network connection clients max concurrent",
                         _network_connection_clients_max_concurrent_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 10000)),
                         100,
                         true),
    
    new c_config_element("network connection clients max message size bytes",
                         _network_connection_clients_max_message_size_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(15, 102400)),
                         10000,
                         true),
    
    new c_config_element("network connection clients max pending",
                         _network_connection_clients_max_pending_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 1024)),
                         25,
                         true),
    
    new c_config_element("network connection port",
                         _network_connection_port_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, LONG_MAX)),
                         9999,
                         true),
    
    new c_config_element("network connection read retry",
                         _network_connection_read_retry_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(0, 1000)),
                         19,
                         true),
    
    new c_config_element("network connection read timeout ms",
                         _network_connection_read_timeout_ms_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 600000)),
                         1000,
                         true),
    
    new c_config_element("network connection write retry",
                         _network_connection_write_retry_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(0, 1000)),
                         10,
                         true),
    
    new c_config_element("network connection write timeout ms",
                         _network_connection_write_timeout_ms_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 600000)),
                         1000,
                         true),
    
    new c_config_element("protocol header size length",
                         _protocol_header_size_length_path,
                         std::unique_ptr<const std::tuple<int64_t, int64_t>>(new std::tuple<int64_t, int64_t>(1, 1024)),
                         15,
                         true)
  };

  auto& class_config_elements { _config_elements };
  
  std::for_each(config_elements.begin(), config_elements.end(), [&class_config_elements](auto& c) { class_config_elements.push_back(std::unique_ptr<c_config_element>(c)); } );

	return true;
}

void c_config::help() { // TODO add config file info
	std::cerr << "battleship: "
  << std::endl << "multiplexing network server/client process for battleship game protocol"
  << std::endl << "usage: "
  << std::endl << "battleship -h              # to display this information"
  << std::endl << "battleship -f <filename>   # file containing a json configuration"
  << std::endl;
}

bool c_config::load_config(const std::string &arg_key,
                           const std::map<std::string,
                           std::string> &arguments,
                           OUT std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings) {

	auto iter { arguments.find(arg_key) };

	if (iter == arguments.end()) {
		std::cerr << arg_key << " file argument mandatory" << std::endl;
		return false;
	}

	std::string filename(iter->second);

	c_ifile file(filename);

	if (!file.valid()) {
		std::cerr << "could not load file " << filename << std::endl;
		return false;
	}

	try {
		c_config::_json->operator=(nlohmann::json::parse(file.os().str().c_str()));
	} catch (...) {
		std::cerr << "invalid json structure in file " << filename << std::endl;
		return false;
	}

	nlohmann::json j { *(c_config::_json.get()) };
  std::vector<std::string> j_paths;
  
	if (!c_json::extract_configuration_from_json(j, j_paths, settings)) {
		std::cerr << "invalid json data in file " << filename << std::endl;
		return false;
	}

	bool result(true);

	for (auto &setting : settings) {

		if (!c_json::is_configured_and_valid(_config_elements, setting.first, std::get<0>(setting.second),
				std::get<1>(setting.second))) {

			std::cerr << "invalid json at path ";
			for (auto &setting_name : setting.first) {
				std::cerr << setting_name << " ";
			}
			std::cerr << std::endl;

			result = false;
		}
	}

	return result;
}

bool c_config::load_arguments(int32_t argc, const char *argv[], std::map<std::string, std::string> &arguments) {

	if (argc != 1) {

		std::string name;
		std::string value;

		for (uint32_t i(1); i < argc; ++i) {

			std::string token(cc::c_strings::trim(argv[i]));

			if (token.empty() || token == "-h" || token == "-H") {
				_help_requested = true;
				return false;
			}

			if (name.empty()) {
				name = c_strings::to_lower(token);
			} else if (value.empty()) {
				value = token;
			}

			if (!name.empty() && !value.empty()) {
				arguments[name] = value;
				name = value = "";
			}
		}

		if (!name.empty() && value.empty()) {
			std::cerr << name << " argument value missing" << std::endl;
			return false;
		}
	}
  
  if (!c_collections::collection_to_bool(arguments.begin(),
                                         arguments.end(), [](auto &e) { return e.first == "-f" || e.first == "-h"; } )) {
    std::cerr << "invalid arguments - only '-h' and '-f' allowed" << std::endl;
    return false;
  }

  return true;
}

bool c_config::load(int32_t argc, const char *argv[]) {

	std::map<std::string, std::string> arguments;

	if (argc == 2) {

		if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-H")) {
			_help_requested = true;
			return true;
		}

    std::cerr << "invalid arguments - only '-h' and '-f' allowed" << std::endl;
		return false;
	}

	if (!c_config::load_arguments(argc, argv, arguments)) {
		return false;
	}

	std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> settings;

	if (!c_config::load_config("-f", arguments, settings)) {
		return false;
	}

	_config.reset(new c_config(settings));

	return _config->valid();
}
 
c_config::c_config(
		const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings) :
    _logfile(nullptr),
    _process_type(get_process_type(settings, _process_type_path)),
		_network_connection_port(extract_int64_t(settings, _network_connection_port_path )),
		_network_connection_clients_max_pending(extract_int64_t(settings, _network_connection_clients_max_pending_path)),
		_network_connection_buffer_size(extract_int64_t(settings, _network_connection_buffer_size_path)),
    _network_connection_bind_ip(extract_string(settings, _network_connection_bind_ip_path)),
		_network_connection_clients_max_message_size(extract_int64_t(settings, _network_connection_clients_max_message_size_path)),
		_network_connection_clients_max_concurrent(extract_int64_t(settings, _network_connection_clients_max_concurrent_path)),
		_network_connection_read_timeout_ms(extract_int64_t(settings, _network_connection_read_timeout_ms_path )),
    _network_connection_read_retry(extract_int64_t(settings, _network_connection_read_retry_path )),
    _network_connection_write_timeout_ms(extract_int64_t(settings, _network_connection_write_timeout_ms_path )),
    _network_connection_write_retry(extract_int64_t(settings, _network_connection_write_retry_path )),
    _protocol_header_size_length(extract_int64_t(settings, _protocol_header_size_length_path )),
    _capacity_datamodel_pool_size(extract_int64_t(settings, _capacity_datamodel_pool_size_path )),
    _capacity_listener_pool_size(extract_int64_t(settings, _capacity_listener_pool_size_path )),
    _capacity_reader_pool_size(extract_int64_t(settings, _capacity_reader_pool_size_path )),
    _capacity_writer_pool_size(extract_int64_t(settings, _capacity_writer_pool_size_path )),
    _internal_wait_service_exit(extract_int64_t(settings, _internal_wait_service_exit_path )),
    _internal_test_speed_slow(extract_bool(settings, _internal_test_speed_slow_path )),
    _admin_username(extract_string(settings, _admin_username_path )),
    _admin_password(extract_string(settings, _admin_password_path )),
    _diagnostics_log_filename(extract_string(settings, _diagnostics_log_filename_path )),
    _diagnostics_log_level(extract_string(settings, _diagnostics_log_level_path )),
    _diagnostics_log_output(extract_bool(settings, _diagnostics_log_output_path )),
    _diagnostics_log_truncate(extract_bool(settings, _diagnostics_log_truncate_path )),
    _diagnostics_log_console(extract_bool(settings, _diagnostics_log_console_path )),
    _diagnostics_log_dump_connections(extract_bool(settings, _diagnostics_log_dump_connections_path )) {
      
      if (_diagnostics_log_output) {
        _logfile.reset(new cc::c_ofile(diagnostics_log_filename(), diagnostics_log_truncate()));
      }
      
      c_config::_valid = true;
}

// TODO - tidy
c_config::TYPE_PROCESS c_config::get_process_type(
		const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings,
		const std::vector<std::string> &key) {

	auto iter { settings.find(key) };

	if (iter == settings.end()) {
		assert(!"TYPE_PROCESS extract - cannot get here");
	}

	if (*(reinterpret_cast<std::string*>(std::get<1>(iter->second))) == "server") {
		return c_config::TYPE_PROCESS_SERVER;
	}

	return c_config::TYPE_PROCESS_NONE;
}

// TODO - tidy
std::string c_config::extract_string(
		const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings,
		const std::vector<std::string> &key) {

	auto iter { settings.find(key) };

	if (iter == settings.end()) {
		assert(!"extract - cannot get here");
	}

	std::string *data { reinterpret_cast<std::string*>(std::get<1>(iter->second)) };

	return (*data);
}

// TODO - tidy
int64_t c_config::extract_int64_t(
		const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings,
		const std::vector<std::string> &key) {

	auto iter { settings.find(key) };

	if (iter == settings.end()) {
		assert(!"extract - cannot get here");
	}

  int64_t *data { reinterpret_cast<int64_t*>(std::get<1>(iter->second)) };

	return (*data);
}

// TODO - tidy
double c_config::extract_double(
		const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings,
		const std::vector<std::string> &key) {

	auto iter { settings.find(key) };

	if (iter == settings.end()) {
		assert(!"extract - cannot get here");
	}

	double *data { reinterpret_cast<double*>(std::get<1>(iter->second)) };

	return (*data);
}

// TODO - tidy
bool c_config::extract_bool(
		const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings,
		const std::vector<std::string> &key) {

	auto iter { settings.find(key) };

	if (iter == settings.end()) {
		assert(!"extract - cannot get here");
	}

	bool *data { reinterpret_cast<bool*>(std::get<1>(iter->second)) };

	return (*data);
}

}

