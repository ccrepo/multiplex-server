//
// cc_config.h
//

#ifndef _CC_CONFIG_H
#define _CC_CONFIG_H

#include <memory>
#include <string>
#include <map>
#include <vector>

#include "cc_json.h"
#include "cc_config_element.h"
#include "cc_file.h"

// TODO rewrite and simplify config
// TODO optional values, null values, wide characters

namespace cc {

class c_config final : public std::enable_shared_from_this<c_config> {

public:
	static bool init(int32_t argc, const char *argv[]);

	static void help();

	static bool help_requested() {
		return _help_requested;
	}

	static const c_config* singleton() {
		return _config.get();
	}
  
  static bool valid() {
    return _valid.load();
  }

public:
	enum TYPE_PROCESS {
		TYPE_PROCESS_NONE, TYPE_PROCESS_SERVER
	};

public:
  inline c_config::TYPE_PROCESS process_type() const {
		return _process_type;
	}

  inline int32_t network_connection_port() const {
		return static_cast<int32_t>(_network_connection_port);
	}

  inline int32_t network_connection_clients_max_pending() const {
    return static_cast<int32_t>(_network_connection_clients_max_pending);
	}

  inline int32_t network_connection_buffer_size() const {
    return static_cast<int32_t>(_network_connection_buffer_size);
	}

  inline const std::string& network_connection_bind_ip() const {
    return _network_connection_bind_ip;
  }

  inline int64_t network_connection_clients_max_message_size() const {
		return _network_connection_clients_max_message_size;
	}

  inline int32_t network_connection_clients_max_concurrent() const {
    return static_cast<int32_t>(_network_connection_clients_max_concurrent);
	}

  inline int32_t network_connection_read_timeout_ms() const {
		return static_cast<int32_t>(_network_connection_read_timeout_ms);
	}

  inline int32_t network_connection_read_retry() const {
    return static_cast<int32_t>(_network_connection_read_retry);
	}

  inline int32_t network_connection_write_timeout_ms() const {
		return static_cast<int32_t>(_network_connection_write_timeout_ms);
	}

  inline int32_t network_connection_write_retry() const {
    return static_cast<int32_t>(_network_connection_write_retry);
	}

  inline int32_t protocol_header_size_length() const {
    return static_cast<int32_t>(_protocol_header_size_length);
	}

  inline int32_t capacity_datamodel_pool_size() const {
    return static_cast<int32_t>(_capacity_datamodel_pool_size);
	}

  inline int32_t capacity_listener_pool_size() const {
    return static_cast<int32_t>(_capacity_listener_pool_size);
  }
  
  inline int32_t capacity_reader_pool_size() const {
    return static_cast<int32_t>(_capacity_reader_pool_size);
  }

  inline int32_t capacity_writer_pool_size() const {
    return static_cast<int32_t>(_capacity_writer_pool_size);
  }

  inline int32_t internal_wait_service_exit() const {
    return static_cast<int32_t>(_internal_wait_service_exit);
  }

  inline bool internal_test_speed_slow() const {
    return _internal_test_speed_slow;
  }

  inline const std::string& admin_username() const {
		return _admin_username;
	}

  inline const std::string& admin_password() const {
		return _admin_password;
	}

  inline const std::string& diagnostics_log_filename() const {
		return _diagnostics_log_filename;
	}

  inline bool diagnostics_log_output() const {
    return _diagnostics_log_output;
  }

  inline bool diagnostics_log_truncate() const {
    return _diagnostics_log_truncate;
  }
  
  inline const std::string& diagnostics_log_level() const {
		return _diagnostics_log_level;
	}

  inline bool diagnostics_log_console() const {
		return _diagnostics_log_console;
	}

  inline bool diagnostics_log_dump_connections() const {
    return _diagnostics_log_dump_connections;
  }

  inline const std::shared_ptr<cc::c_ofile>& logfile() const {
    return _logfile;
  };
  
  inline int32_t maintenance_warning_count() const { // TODO read from config
    return 10;
  }
  
public:
	~c_config() { 
	}

private:
	static bool load(int32_t argc, const char *argv[]);

	static bool load_arguments(int32_t argc, const char *argv[], std::map<std::string, std::string> &arguments);

	static bool load_config(const std::string &arg_key,
                          const std::map<std::string, std::string> &arguments,
                          std::map<std::vector<std::string>,
                          OUT std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings);

	static std::unique_ptr<nlohmann::json> _json;

	static bool configure();

private:
	static std::shared_ptr<c_config> _config;
	static bool _help_requested;

private:
	static std::vector<std::unique_ptr<c_config_element>> _config_elements;
  static std::atomic<bool> _valid;

private:
  std::shared_ptr<cc::c_ofile> _logfile;

	const TYPE_PROCESS _process_type;

	const int64_t _network_connection_port;
	const int64_t _network_connection_clients_max_pending;
	const int64_t _network_connection_buffer_size;
	const int64_t _network_connection_clients_max_message_size;
	const int64_t _network_connection_clients_max_concurrent;
	const int64_t _network_connection_read_timeout_ms;
	const int64_t _network_connection_read_retry;
	const int64_t _network_connection_write_timeout_ms;
	const int64_t _network_connection_write_retry;
	const int64_t _protocol_header_size_length;
	const int64_t _capacity_datamodel_pool_size;
  const int64_t _capacity_listener_pool_size;
  const int64_t _capacity_reader_pool_size;
  const int64_t _capacity_writer_pool_size;
  const int64_t _internal_wait_service_exit;
  
	const std::string _admin_username;
	const std::string _admin_password;
	const std::string _diagnostics_log_filename;
	const std::string _diagnostics_log_level;
  const std::string _network_connection_bind_ip;
  
	const bool _diagnostics_log_console;
  const bool _diagnostics_log_dump_connections;
  const bool _diagnostics_log_output;
  const bool _diagnostics_log_truncate;
  const bool _internal_test_speed_slow;
  
private:
	c_config(const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings);

	TYPE_PROCESS get_process_type(
			const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &settings,
			const std::vector<std::string> &key);

	std::string extract_string(
			const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &setting,
			const std::vector<std::string> &key);

  int64_t extract_int64_t(
			const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &setting,
			const std::vector<std::string> &key);

	double extract_double(
			const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &setting,
			const std::vector<std::string> &key);

	bool extract_bool(
			const std::map<std::vector<std::string>, std::tuple<c_config_element::CONFIG_ELEMENTYPE, void*>> &setting,
			const std::vector<std::string> &key);

private:
	inline static std::vector<std::string> _process_type_path { "process", "type" };
	inline static std::vector<std::string> _network_connection_port_path { "network", "connection", "port" };
	inline static std::vector<std::string> _network_connection_clients_max_pending_path { "network", "connection", "clients", "max", "pending" };
	inline static std::vector<std::string> _network_connection_buffer_size_path { "network", "connection", "buffer_size" };
  inline static std::vector<std::string> _network_connection_bind_ip_path { "network", "connection", "bind_ip" };
	inline static std::vector<std::string> _network_connection_clients_max_message_size_path { "network", "connection", "clients", "max", "message_size" };
	inline static std::vector<std::string> _network_connection_clients_max_concurrent_path { "network", "connection", "clients", "max", "concurrent" };
	inline static std::vector<std::string> _network_connection_read_timeout_ms_path { "network", "connection", "read", "timeout"  };
	inline static std::vector<std::string> _network_connection_read_retry_path { "network", "connection", "read", "retry" };
	inline static std::vector<std::string> _network_connection_write_timeout_ms_path { "network", "connection", "write", "timeout"  };
	inline static std::vector<std::string> _network_connection_write_retry_path { "network", "connection", "write", "retry"  };
	inline static std::vector<std::string> _protocol_header_size_length_path { "protocol", "header", "size", "length"  };
	inline static std::vector<std::string> _capacity_datamodel_pool_size_path { "capacity", "datamodel", "pool", "size" };
  inline static std::vector<std::string> _capacity_listener_pool_size_path { "capacity", "listener", "pool", "size" };
  inline static std::vector<std::string> _capacity_reader_pool_size_path { "capacity", "reader", "pool", "size" };
  inline static std::vector<std::string> _capacity_writer_pool_size_path { "capacity", "writer", "pool", "size" };
  inline static std::vector<std::string> _internal_wait_service_exit_path { "internal", "wait", "service", "exit" };
  inline static std::vector<std::string> _internal_test_speed_slow_path { "internal", "test", "speed", "slow" };

  inline static std::vector<std::string> _admin_username_path { "admin", "username"  };
	inline static std::vector<std::string> _admin_password_path { "admin", "password" };
	inline static std::vector<std::string> _diagnostics_log_filename_path { "diagnostics", "log", "filename" };
  inline static std::vector<std::string> _diagnostics_log_output_path { "diagnostics", "log", "output" };
  inline static std::vector<std::string> _diagnostics_log_truncate_path { "diagnostics", "log", "truncate" };
	inline static std::vector<std::string> _diagnostics_log_level_path { "diagnostics", "log", "level"  };
	inline static std::vector<std::string> _diagnostics_log_console_path { "diagnostics", "log", "console"  };
  inline static std::vector<std::string> _diagnostics_log_dump_connections_path { "diagnostics", "log", "dump_connections"  };

private:
	c_config& operator=(const c_config&) = delete;
	c_config(const c_config&) = delete;
};

}

#endif /* _CC_CONFIG_H */
