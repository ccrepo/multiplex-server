//
// cc_bools.h
//

#ifndef _CC_BOOLS_H
#define _CC_BOOLS_H

namespace cc {

class c_bools final {

public:
  
  inline static const std::string& to_string(bool v) {
    static const std::string t("true");
    static const std::string f("false");

    return v ? t : f;
  }
  
private:
  c_bools() = delete;
  c_bools(c_bools&) = delete;
  c_bools& operator=(c_bools&) = delete;
	~c_bools() = delete;
};

}

#endif /* _CC_BOOLS_H */
