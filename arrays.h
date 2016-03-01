#include "types.h"
#ifndef ARRAYS_H_
#define ARRAYS_H_

namespace arrow {


template<typename T>
class Array
{
public:
    typedef typename T::value_type value_type;

    Array(const value_type* values, int32_t length) : length_(length),
                                                            values_(values) {}

    const value_type get(int32_t slotNumber) const {
      return values_[slotNumber];
    }

    // implemented so Sum() can work on both
    const value_type get(int32_t slotNumber, const value_type null_value) const {
      return get(slotNumber);
    }

    const int32_t length() const { return length_; }

    const int32_t null_count() const { return 0; }

    std::string to_string() const {
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
    const value_type* values_;
    const int32_t length_;
};

// Nullable array. Declared with reference to any Nullable Type
// i.e. Array< Nullable< PrimitiveType<uint32_t> > >
template<typename T>
class Array< Nullable<T> >
{
public:
  typedef Array<T> ChildArrayType;
  typedef typename T::value_type value_type;

  Array(const ChildArrayType &child_array, const bool* null_bitmask, const int32_t null_count) : child_array_(child_array),
                                                                                                 length_(child_array_.length()), 
                                                                                                 null_bitmask_(null_bitmask),
                                                                                                 null_count_(null_count) {
  }

  const int32_t null_count() const {
    return null_count_;
  }

  const bool no_nulls() const {
    return null_count_ == 0;
  }

  // doesn't do null check
  const value_type get(int32_t slotNumber) const {
    return child_array_.get(slotNumber);
  }

  const value_type get(int32_t slotNumber, const value_type null_value) const {
    return is_null(slotNumber) ? null_value : child_array_.get(slotNumber, null_value);
  }

  const bool is_null(int32_t slotNumber) const {
    return null_bitmask_[slotNumber];
  }

  const ChildArrayType& child_array() const {
    return child_array_;
  }

  std::string to_string() const {
    std::stringstream s;
    s << "Array [";
    for (int32_t slotNumber = 0; slotNumber < length_; slotNumber++) {
      if (slotNumber) s << ", ";
      s << ((is_null(slotNumber)) ? "null" : std::to_string(child_array_.get(slotNumber)));
    }
    s << "]";
    return s.str();
  }

  const int32_t length() const {
    return length_;
  }

private:
  const ChildArrayType child_array_;
  const bool* null_bitmask_;
  const int32_t length_;
  const int32_t null_count_;
};

} // namespace arrow

#endif