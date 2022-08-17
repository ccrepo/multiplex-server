//
// cc_attributes_id.h
//

#ifndef _CC_ATTRIBUTES_ID_H
#define _CC_ATTRIBUTES_ID_H

#include <memory>

namespace cc {

class c_attributes_id {

public:
  c_attributes_id() : _attributes_id(_attributes_id_counter.fetch_add(1, std::__1::memory_order_relaxed))
  {}
  
  virtual ~c_attributes_id()
  {}

  inline uint64_t attributes_id() const {
    return _attributes_id;
  }
  
private:
  const uint64_t _attributes_id { false };
  inline static std::atomic<uint64_t> _attributes_id_counter { 0 };
  
private:
  c_attributes_id(const c_attributes_id&) = delete;
  c_attributes_id& operator=(const c_attributes_id&) = delete;
  
};

}

#endif /* _CC_ATTRIBUTES_ID_H */

