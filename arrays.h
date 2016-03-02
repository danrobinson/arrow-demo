#ifndef ARRAYS_H_
#define ARRAYS_H_
#include "types.h"

namespace arrow {


template<typename T>
class Array {};

template<typename T>
class Array<PrimitiveType<T> >
{
public:
    using array_type = PrimitiveType<T>;
    using value_type = typename array_type::value_type;

    Array(const value_type* values, int32_t length) : length_(length),
                                                            values_(values) {}

    const value_type get(int32_t slotNumber) const {
      return values_[slotNumber];
    }

    // implemented so Sum() can work on both
    const value_type get(int32_t slotNumber, const value_type null_value) const {
      return get(slotNumber);
    }

    int32_t length() const { return length_; }

    // these are here so you can write a type-ignorant algorithm if you WANT To
    int32_t null_count() const { return 0; }
    int32_t no_nulls() const { return true; }
    int32_t is_null(int32_t slotNumber) const { return false; }

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

    const value_type* values() const {
      return values_;
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
  using array_type = Nullable<T>;
  using value_type = typename array_type::value_type;

  Array(const Array<T> &child_array, const bool* null_bitmask, const int32_t null_count) : child_array_(child_array),
                                                                                                 length_(child_array_.length()), 
                                                                                                 null_bitmask_(null_bitmask),
                                                                                                 null_count_(null_count) {
  }

  int32_t null_count() const {
    return null_count_;
  }

  bool no_nulls() const {
    return null_count_ == 0;
  }

  const value_type get(int32_t slotNumber) const {
    return child_array_.get(slotNumber);
  }

  const value_type get(int32_t slotNumber, const value_type null_value) const {
    return is_null(slotNumber) ? null_value : child_array_.get(slotNumber, null_value);
  }

  bool is_null(int32_t slotNumber) const {
    return null_bitmask_[slotNumber];
  }

  const Array<T>& child_array() const {
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

  int32_t length() const {
    return length_;
  }

private:
  const Array<T> child_array_;
  const bool* null_bitmask_;
  const int32_t length_;
  const int32_t null_count_;
};

// THIS IS NOT NECESSARY. The above one will work for all types including primitive types
// But we implement it for a little efficiency benefit
template<typename T>
class Array<Nullable<PrimitiveType<T> > >
{
public:
  using child_type = PrimitiveType<T>;
  using array_type = Nullable<child_type>;
  using value_type = typename array_type::value_type;

  Array(const Array<child_type> &child_array, const bool* null_bitmask, const int32_t null_count) : child_array_(child_array),
                                                                                           values_(child_array_.values()),
                                                                                           length_(child_array_.length()), 
                                                                                           null_bitmask_(null_bitmask),
                                                                                           null_count_(null_count) {
  }

  int32_t null_count() const {
    return null_count_;
  }

  bool no_nulls() const {
    return null_count_ == 0;
  }

  const value_type get(int32_t slotNumber) const {
    return values_[slotNumber];
  }

  const value_type get(int32_t slotNumber, const value_type null_value) const {
    return is_null(slotNumber) ? null_value : values_[slotNumber];
  }

  bool is_null(int32_t slotNumber) const {
    return null_bitmask_[slotNumber];
  }

  const Array<child_type>& child_array() const {
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

  int32_t length() const {
    return length_;
  }

private:
  const Array<child_type> child_array_;
  const value_type* values_;
  const bool* null_bitmask_;
  const int32_t length_;
  const int32_t null_count_;
};


// traits

template<typename T>
class is_nullable_array {
public:
  static const bool value = false;
};

template<typename T>
class is_nullable_array<Array<Nullable<T> > > {
public:
  static const bool value = true;
};

template<typename T>
using if_nullable_array = typename std::enable_if<is_nullable_array<T>::T>;


} // namespace arrow

#endif