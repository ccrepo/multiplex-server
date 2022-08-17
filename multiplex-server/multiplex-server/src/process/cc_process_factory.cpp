//
// cc_process_factory.cpp
//

#include <cassert>

#include "cc_process_factory.h"
#include "cc_server.h"

namespace cc {

std::shared_ptr<c_process> c_process_factory::get_process(c_config::TYPE_PROCESS type_process) {

	switch (type_process) {

	case c_config::TYPE_PROCESS_SERVER: {
		return std::shared_ptr < c_process > (new c_server());
		break;
	}

	default:
		assert(!"TYPE_PROCESS allocation should not get here as all enums handled above");
	}

	return std::shared_ptr<c_process>();
}

}

