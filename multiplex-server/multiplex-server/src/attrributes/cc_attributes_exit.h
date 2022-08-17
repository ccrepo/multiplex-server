//
// cc_attributes_exit.h
//

#ifndef _CC_ATTRIBUTES_EXIT_H
#define _CC_ATTRIBUTES_EXIT_H

#include <memory>

namespace cc {

class c_attributes_exit {

public:
  c_attributes_exit() {}
  virtual ~c_attributes_exit() {}

  inline bool is_instance_exit_set_seq_cst() const {
    return _instance_exit_flag.load();
  }

  inline bool is_instance_exit_set_relaxed() const {
    return _instance_exit_flag.load(std::memory_order_relaxed);
  }

  inline void set_instance_exit_flag() {
    _instance_exit_flag.store(true);
  }

  std::function<void ()> get_set_instance_exit_lambda() {
    return [this]() {
      this->set_instance_exit_flag();
    };
  }
  
  inline static bool is_global_exit_set_seq_cst() {
    return _global_exit_flag.load();
  }

  inline static bool is_global_exit_set_relaxed() {
    return _global_exit_flag.load(std::memory_order_relaxed);
  }

  static void set_global_exit_flag() {
    _global_exit_flag.store(true);
  }
  
  inline bool is_attribute_exit_any() const {
    if (is_instance_exit_set_relaxed() &&
        is_instance_exit_set_seq_cst()) {
      return true;
    }

    if (c_attributes_exit::is_global_exit_set_relaxed() &&
        c_attributes_exit::is_global_exit_set_seq_cst()) {
      return true;
    }

    return false;
  }
  
  inline bool is_attribute_exit_instance() const {
    if (is_instance_exit_set_relaxed() &&
        is_instance_exit_set_seq_cst()) {
      return true;
    }

    return false;
  }
  
  inline static bool is_attribute_exit_global() {
    if (c_attributes_exit::is_global_exit_set_relaxed() &&
        c_attributes_exit::is_global_exit_set_seq_cst()) {
      return true;
    }
    
    return false;
  }
  
private:
  std::atomic<bool> _instance_exit_flag { false };
  inline static std::atomic<bool> _global_exit_flag { false };
  
private:
  c_attributes_exit(const c_attributes_exit&) = delete;
  c_attributes_exit& operator=(const c_attributes_exit&) = delete;
  
};

}

#endif /* _CC_ATTRIBUTES_EXIT_H */

