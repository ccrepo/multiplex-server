//
// multiplex-server.cpp
//

// TODO remove xcode and maybe port to linux
// TODO fix bug for missing config
// TODO valgrind
// TODO datagrams, serialisation, encryption, compression
// TODO IPv6
// TODO generalise state-transition and db action
// TODO replace waits with cond var sets
// TODO improve code around possible failures and failure management
// TODO more stress-testing

#include <iostream>
#include <csignal>
#include <memory>

#include "cc_log.h"
#include "cc_process.h"
#include "cc_process_factory.h"
#include "cc_config.h"
#include "cc_networks.h"

void signal_handler(int signal_num) {
	std::cout << "signal caught [" << signal_num << "] !!" << std::endl;
	std::cout << "exiting ..." << std::endl;
	std::cout.flush();
  
  cc::c_attributes_exit::set_global_exit_flag();
}

int main(int argc, const char *argv[]) {
  
  signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	
	if (!cc::c_config::init(argc, argv)) {
		std::cerr << "bad arguments" << std::endl;
    for (uint32_t idx(0); idx<argc; ++idx) {
      std::cerr << "argv[" << idx << "] = " << argv[idx] << std::endl;
    }
		return 1;
	}

  if (cc::c_config::help_requested()) {
    cc::c_config::help();
    return 0;
  }

	if (!cc::c_config::singleton()->valid()) {
		std::cerr << "config not valid" << std::endl;
		return 1;
	}

	LOG_INFO("starting ...");

  {
    std::shared_ptr<cc::c_process> process(cc::c_process_factory::get_process(cc::c_config::singleton()->process_type()));
    
    if (!process.get()) {
      LOG_ERROR("factory returned null process");
      return 1;
    }
    
    if (!process->start_process()) {
      LOG_ERROR("could not start process");
      return 1;
    }
    
    while (!process->is_attribute_exit_any()) {
      LOG_TRACE("main looping");
      
      if (!process->do_process_maintenance()) {
        LOG_ERROR("diagnostics failed");
        
        cc::c_attributes_exit::set_global_exit_flag();
        break;
      }
      
      if (cc::c_config::singleton()->internal_test_speed_slow()) { // TODO cond var
        cc::c_sleep::sleep_for_random_in_range(cc::c_duration::_5000_ms, cc::c_duration::_10000_ms);
      } else {
        cc::c_sleep::sleep_for_random_in_range(cc::c_duration::_250_ms, cc::c_duration::_500_ms);
      }
    }
    
    if (!process->stop_process()) {
      LOG_ERROR("could not stop process");
      return 1;
    }
  }
  
	LOG_INFO("fini.");
	return 0;
}

