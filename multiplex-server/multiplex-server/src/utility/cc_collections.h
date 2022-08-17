//
// cc_collections.h
//

#ifndef _CC_COLLECTIONS_H
#define _CC_COLLECTIONS_H

#include <algorithm>
#include <vector>

namespace cc {

class c_collections final {

public:

  template<typename InputIt>
  static std::string collection_to_string(InputIt begin, InputIt end, char delim='|') {
    
    std::ostringstream os;
    
    std::for_each(begin, end, [&os, &delim](const auto& elem) {
      if(!os.str().empty())
        os << delim;
      os << elem << std::endl;
    });
    
    return os.str();
  }

  template<typename InputIt>
  static bool collection_to_bool(InputIt begin,
                                        InputIt end,
                                        const std::function<bool(const typename std::iterator_traits<InputIt>::value_type& type)>& func) {
    
    using value_type = typename std::iterator_traits<InputIt>::value_type;
    
    bool result(true);

    std::for_each(begin, end, [&result, &func](const value_type& elem) {
      result = result && func(elem);
    });

    return result;
  }

private:
  c_collections() = delete;
  c_collections(c_collections&) = delete;
  c_collections& operator=(c_collections&) = delete;
	~c_collections() = delete;
};

}

#endif /* _CC_COLLECTIONS_H */
