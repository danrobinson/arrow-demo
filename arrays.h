#include "types.h"

namespace arrow {

// Array interface
template<typename T>
class ArrayInterface
{
public:
  typedef typename T::value_type value_type;
  ArrayInterface(int32_t length) : length_(length) {}
  virtual ~ArrayInterface() {}

  // get a value at a particular slot
  value_type get(int32_t slotNumber) {
    check_bounds(slotNumber);
    return safe_get(slotNumber);       
  }

  // overloaded version that can take a default value to return for null values
  value_type get(int32_t slotNumber, value_type null_value) {
    check_bounds(slotNumber);
    return safe_get(slotNumber, null_value);
  }

  void check_bounds(int32_t slotNumber) {
    if (slotNumber >= length() || slotNumber < 0) throw std::out_of_range( "index " + std::to_string(slotNumber) + " out of range" );
  }

  // gets value after length check
  virtual value_type safe_get(int32_t slotNumber) = 0;
  virtual value_type safe_get(int32_t slotNumber, value_type null_value) {
    return safe_get(slotNumber);
  }

  int32_t length() { return length_; }

  virtual std::string to_string() {
    std::stringstream s;
    s << "Array [";
    for (int32_t slotNumber = 0; slotNumber < length_; slotNumber++) {
      if (slotNumber) s << ", ";
      s << this->string_value(slotNumber);
    }
    s << "]";
    return s.str();
  }

  virtual bool isNull(int32_t slotNumber) { return false; }

  virtual int32_t null_count() { return 0; }
private:
  int32_t length_;  

  virtual std::string string_value(int32_t slotNumber) {
    return std::to_string(safe_get(slotNumber));
  }
};

template<typename T>
class Array : public ArrayInterface<T>
{
public:
    typedef typename T::value_type value_type;

    Array(value_type* values, int32_t length) : ArrayInterface<T>(length) {
      values_ = values;
    }

    value_type safe_get(int32_t slotNumber) {
      return values_[slotNumber];
    }

    value_type safe_get(int32_t slotNumber, value_type null_value) {
      return safe_get(slotNumber);
    }

private:
    value_type* values_;

};

// Nullable array. Declared with reference to any Nullable Type
// i.e. Array< Nullable< PrimitiveType<uint32_t> > >
// has the same interface as non-nullable array, but different behavior
template<typename T>
class Array< Nullable<T> > : public ArrayInterface<T>
{
public:
  typedef Array<T> ChildArrayType;
  typedef typename T::value_type value_type;

  Array(ChildArrayType* child_array, bool* null_bitmask) : ArrayInterface<T>(child_array->length()), child_array_(child_array) {
    null_bitmask_ = null_bitmask;
    null_count_ = 0;
    int32_t len = this->length();
    for (int slotNumber = 0; slotNumber < len; slotNumber++) {
      null_count_ += null_bitmask_[slotNumber];
    }
  }

  int32_t null_count() {
    return null_count_;
  }

  value_type safe_get(int32_t slotNumber) {
    if (null_bitmask_[slotNumber]) { 
      throw std::domain_error( "value at index " + std::to_string(slotNumber) + " is null" );
    } else {
      return child_array_->safe_get(slotNumber);      
    }
  }

  value_type safe_get(int32_t slotNumber, value_type null_value) {
    return isNull(slotNumber) ? null_value : child_array_->get(slotNumber, null_value);
  }

  bool isNull(int32_t slotNumber) {
    return null_bitmask_[slotNumber];
  }

  ChildArrayType* child_array() {
    return child_array_;
  }

private:
  ChildArrayType* child_array_;
  bool *null_bitmask_;
  int32_t null_count_;

  std::string string_value(int32_t slotNumber) {
    return isNull(slotNumber) ? "null" : std::to_string(child_array_->safe_get(slotNumber));
  }
};

} // namespace arrow