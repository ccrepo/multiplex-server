//
// cc_message_bad_request.h
//

#ifndef _CC_MESSAGE_BADREQUEST_H
#define _CC_MESSAGE_BADREQUEST_H

#include "cc_json.h"
#include "cc_log.h"
#include "cc_message.h"

namespace cc {

class c_message_bad_request final : public c_message {
  
public:
  c_message_bad_request() : c_message() {
    _j["body"] = "invalid request";
  }
  
  ~c_message_bad_request()
  {}
  
  c_message_bad_request& operator=(const c_message_bad_request& message) {
    static_cast<c_message*>(this)->operator=(message);
    return (*this);
  }
  
  c_message_bad_request(const c_message_bad_request& message) : c_message(message)
  {}
};

}

#endif /* _CC_MESSAGE_BADREQUEST_H */

