#include "types.h"
#ifndef ARRAYS_H_
#define ARRAYS_H_

namespace arrow {


template<typename T>
class Array
{
public:
    typedef typename T::value_type value_type;

    Array(value_type* values, int32_t length) : length_(length),
                                                values_(values) {}

    value_type get(int32_t slotNumber) {
      return values_[slotNumber];
    }

    // implemented so Sum() can work on both
    value_type get(int32_t slotNumber, value_type null_value) {
      return get(slotNumber);
    }

    int32_t length() { return length_; }

    int32_t null_count() { return 0; }

    std::string to_string() {
      std::stringstream s;
      s << "Array [";
      for (int32_t slotNumber = 0; slotNumber < length_; slotNumber++) {
        if (slotNumber) s << ", ";
        s << get(slotNumber);
      }
      s << "]";
      return s.str();
    }

private:
    value_type* values_;
    int32_t length_;
};

// Nullable array. Declared with reference to any Nullable Type
// i.e. Array< Nullable< PrimitiveType<uint32_t> > >
template<typename T>
class Array< Nullable<T> >
{
public:
  typedef Array<T> ChildArrayType;
  typedef typename T::value_type value_type;

  Array(ChildArrayType* child_array, bool* null_bitmask, int32_t null_count) : child_array_(child_array),
                                                           length_(child_array->length()), 
                                                           null_bitmask_(null_bitmask),
                                                           null_count_(null_count) {
  }

  int32_t null_count() {
    return null_count_;
  }

  bool no_nulls() {
    return null_count_ == 0;
  }

  // doesn't do null check
  value_type get(int32_t slotNumber) {
    return child_array_->get(slotNumber);
  }

  value_type get(int32_t slotNumber, value_type null_value) {
    return is_null(slotNumber) ? null_value : child_array_->get(slotNumber, null_value);
  }

  bool is_null(int32_t slotNumber) {
    return null_bitmask_[slotNumber];
  }

  ChildArrayType* child_array() {
    return child_array_;
  }

  std::string to_string() {
    std::stringstream s;
    s << "Array [";
    for (int32_t slotNumber = 0; slotNumber < length_; slotNumber++) {
      if (slotNumber) s << ", ";
      s << ((is_null(slotNumber)) ? "null" : std::to_string(child_array_->get(slotNumber)));
    }
    s << "]";
    return s.str();
  }

  int32_t length() {
    return length_;
  }

private:
  ChildArrayType* child_array_;
  int32_t length_;
  bool *null_bitmask_;
  int32_t null_count_;
};

} // namespace arrow

#endif