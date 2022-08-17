//
// cc_message.h
//

#ifndef _CC_MESSAGE_H
#define _CC_MESSAGE_H

#include "cc_json.h"
#include "cc_log.h"

namespace cc {

class c_message {

public:
  ~c_message() {}

  c_message(const c_message &message) {
    this->_j = message._j;
  }
  
  c_message& operator=(const c_message& message) {
    this->_j = message._j;
    return (*this);
  }

  const std::string& to_string() const {
    static const std::string buffer(_j.dump());
    return buffer;
  }

protected:
  c_message() :
  _j() {
  }

protected:
  nlohmann::json _j;
};

}

#endif /* _CC_MESSAGE_H */

