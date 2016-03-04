#ifndef ARRAYS_H_
#define ARRAYS_H_
#include "types.h"

namespace arrow {

class BaseArray {
public:
  BaseArray(int32_t length, int32_t null_count) : length_(length),
                                                  null_count_(null_count) {}

  int32_t null_count() const { return null_count_; }
  bool no_nulls() const { return null_count_ == 0; }
  int32_t length() const { return length_; }
protected:
  const int32_t null_count_;
  const int32_t length_;
};

template<typename T>
class Array {};

template<typename T>
class Array<PrimitiveType<T> > : public BaseArray
{
public:
    using value_type = PrimitiveType<T>;
    using c_type = typename value_type::c_type;

    Array(const c_type* values, int32_t length) : BaseArray(length, 0),
                                                  values_(values) {}

    const c_type get(int32_t slotNumber) const {
      return values_[slotNumber];
    }

    // implemented so Sum() can work on both
    const c_type get(int32_t slotNumber, const c_type null_value) const {
      return get(slotNumber);
    }

    // this is here so you can write a type-ignorant algorithm if you WANT To
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

    const c_type* values() const {
      return values_;
    }

private:
    const c_type* values_;
};

// Nullable array. Declared with reference to any Nullable Type
// i.e. Array< Nullable< PrimitiveType<uint32_t> > >
template<typename T>
class Array< Nullable<T> > : public BaseArray
{
public:
  using value_type = Nullable<T>;
  using c_type = typename value_type::c_type;

  Array(const Array<T> &child_array, const bool* null_bitmask, const int32_t null_count) :  BaseArray(child_array.length(), null_count),
                                                                                            child_array_(child_array),
                                                                                            null_bitmask_(null_bitmask) {}

  const c_type get(int32_t slotNumber) const {
    return child_array_.get(slotNumber);
  }

  const c_type get(int32_t slotNumber, const c_type null_value) const {
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

private:
  const Array<T> child_array_;
  const bool* null_bitmask_;
};

// THIS IS NOT NECESSARY. The above one will work for all types including primitive types
// But we implement it for a little efficiency benefit
template<typename T>
class Array<Nullable<PrimitiveType<T> > > : public BaseArray
{
public:
  using child_type = PrimitiveType<T>;
  using value_type = Nullable<child_type>;
  using c_type = typename value_type::c_type;

  Array(const Array<child_type> &child_array, const bool* null_bitmask, const int32_t null_count) :
                                                                                           BaseArray(child_array.length(), null_count),
                                                                                           child_array_(child_array),
                                                                                           values_(child_array.values()),
                                                                                           null_bitmask_(null_bitmask) {
  }

  const c_type get(int32_t slotNumber) const {
    return values_[slotNumber];
  }

  const c_type get(int32_t slotNumber, const c_type null_value) const {
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

private:
  const Array<child_type> child_array_;
  const c_type* values_;
  const bool* null_bitmask_;
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