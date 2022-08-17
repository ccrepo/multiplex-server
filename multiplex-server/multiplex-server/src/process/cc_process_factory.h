//
// cc_process_factory.h
//

#ifndef _CC_PROCESS_FACTORY_H
#define _CC_PROCESS_FACTORY_H

#include "cc_log.h"
#include "cc_config.h"

namespace cc {

class c_process;

class c_process_factory final {
  
public:
	static std::shared_ptr<c_process> get_process(c_config::TYPE_PROCESS type_process);

private:
	c_process_factory() {}

	~c_process_factory() {}

	c_process_factory& operator=(const c_process_factory&) = delete;
	c_process_factory(const c_process_factory&) = delete;
};

}

#endif /* _CC_PROCESS_FACTORY_H */
