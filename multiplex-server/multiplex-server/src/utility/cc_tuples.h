//
// cc_tuples.h
//

#ifndef _CC_TUPLES_H
#define _CC_TUPLES_H

namespace cc {

class c_tuples final {

public:
  template<typename T>
  static std::string tuple_to_string_range(const std::tuple<T, T>& vs) {
    
    using value_type [[maybe_unused]] = typename std::enable_if<
    std::is_same<int64_t, T>::value ||
    std::is_same<int32_t, T>::value ||
    std::is_same<int16_t, T>::value ||
    std::is_same<double,  T>::value>::type;
    
    std::ostringstream os;

    if (std::is_same<T, double>::value || std::get<0>(vs) != LONG_MIN) {
      os << "min: " << std::get<0>(vs);
    }

    if (std::is_same<T, double>::value || std::get<1>(vs) != LONG_MAX) {
      if (!os.str().empty()) {
        os << " ";
      }
      
      os << "max: " << std::get<1>(vs);
    }
    
    return os.str();
  }

private:
  c_tuples() = delete;
  c_tuples(c_tuples&) = delete;
  c_tuples& operator=(c_tuples&) = delete;
  ~c_tuples() = delete;
};

}

#endif /* _CC_TUPLES_H */
