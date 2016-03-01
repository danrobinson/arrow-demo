#include "types.h"
#ifndef ARRAYS_ALT_H
#define ARRAYS_ALT_H

namespace arrow {

template<typename T>
class ArrayAlt
{
public:
  typedef typename T::value_type value_type;

  ArrayAlt(const value_type* values, const bool* null_bitmask, const int32_t length, const int32_t null_count = 0) : values_(values),
                                                                                                                     length_(length), 
                                                                                                                     null_bitmask_(null_bitmask),
                                                                                                                     null_count_(null_count) {}

  value_type get(int32_t slotNumber) {
    return values_[slotNumber];
  }

  bool is_null(int32_t slotNumber) {
    return null_bitmask_[slotNumber];
  }

  bool no_nulls() {
    return null_count_ == 0;
  }

  std::string to_string() {
    std::stringstream s;
    s << "ArrayAlt [";
    for (int32_t slotNumber = 0; slotNumber < length_; slotNumber++) {
      if (slotNumber) s << ", ";
      s << ((is_null(slotNumber)) ? "null" : std::to_string(get(slotNumber)));
    }
    s << "]";
    return s.str();
  }

  int32_t length() {
    return length_;
  }

  int32_t null_count() {
    return null_count_;
  }

private:
  const value_type* values_;
  const int32_t length_;
  const bool *null_bitmask_;
  const int32_t null_count_;
};

} // namespace arrow

#endif