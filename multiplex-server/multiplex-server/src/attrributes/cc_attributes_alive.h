//
// cc_attributes_alive.h
//

#ifndef _CC_ATTRIBUTES_ALIVE_H
#define _CC_ATTRIBUTES_ALIVE_H

#include <memory>

namespace cc {

class c_attributes_alive {

public:
  c_attributes_alive() {}
  virtual ~c_attributes_alive() {}

  inline bool is_instance_alive_seq_cst() const {
    return _instance_alive_flag.load();
  }

  inline bool is_instance_alive_relaxed() const {
    return _instance_alive_flag.load(std::memory_order_relaxed);
  }

  void set_instance_not_alive_flag() {
   _instance_alive_flag.store(false);
  }
  
  inline bool is_attribute_alive_instance() const {
    if (!is_instance_alive_relaxed() &&
        !is_instance_alive_seq_cst()) {
      return false;
    }

    return true;
  }
  
protected:
  std::function<void ()> get_set_instance_not_alive_lambda() {
    return [this]() {
      this->set_instance_not_alive_flag();
    };
  }

private:
  std::atomic<bool> _instance_alive_flag { true };
  
private:
  c_attributes_alive(const c_attributes_alive&) = delete;
  c_attributes_alive& operator=(const c_attributes_alive&) = delete;
};

}

#endif /* _CC_ATTRIBUTES_ALIVE_H */
