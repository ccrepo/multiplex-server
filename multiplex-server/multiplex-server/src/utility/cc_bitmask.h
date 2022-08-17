//
// cc_bitmask.h
//

#ifndef _CC_BITMASK_H
#define _CC_BITMASK_H

namespace cc {

class c_bitmask final {
  
public:
  c_bitmask operator|(const c_bitmask& value) const {
    return c_bitmask(_value | value._value);
  }

  c_bitmask operator&(const c_bitmask& value) const {
    return c_bitmask(_value & value._value);
  }

  explicit operator bool() const {
    return _value > 0;
  }
  
  ~c_bitmask() {}

private:
  explicit c_bitmask(uint value) : _value(value) {
  }
  
  explicit c_bitmask(const c_bitmask& value) : _value(value._value) {
  }
  
  c_bitmask& operator=(const c_bitmask& value)  {
    _value = value._value;
    return (*this);
  }
  
private:
  uint _value;
};

}

#endif /* _CC_BITMASK_H */
