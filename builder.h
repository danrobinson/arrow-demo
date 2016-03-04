#ifndef BUILDER_H_
#define BUILDER_H_

#include "arrays.h"

namespace arrow {

template<typename T>
class ArrayBuilder{};

template<typename T>
class ArrayBuilder<PrimitiveType<T> >
{
public:
  using array_type = PrimitiveType<T>;
  using return_type = Array<array_type>;
  using c_type = typename array_type::c_type;

  ArrayBuilder(int32_t length) : length_(length),
                                 offset_(0),
                                 values_(new c_type[length]) {}

  void add(const c_type value) {
    values_[offset_++] = value;
  }

  void step(const c_type value) {
    values_[offset_++] = value;
  }

  // manually set value at an offset
  void set(const c_type value, uint32_t offset) {
    values_[offset] = value;
  }

  void add_null() { 
    // SHOULD NEVER BE CALLED. Only here so ignorant algorithms will compile
    // if code calls this, though, it should be refactored
  }

  const int32_t length() { return length_; }

  const return_type* build() {
    return new return_type(values_, length_);
  }

  const return_type* complete() {
    return build();
  }

private:
  const int32_t length_;
  int32_t offset_;
  c_type* values_;
};


// generic builder for any nullable type
// this will work for primitives, but I define one for primitives below for a slight efficiency gain
template<typename T>
class ArrayBuilder<Nullable<T> >
{
public:
  using array_type = Nullable<T>;
  using return_type = Array<array_type>;
  using child_type = T;
  using c_type = typename array_type::c_type;

  ArrayBuilder(int32_t length) : length_(length),
                                 offset_(0),
                                 childBuilder_(length),
                                 nulls_(new bool[length]),
                                 null_count_(0) {}

  void add(const c_type value) {
    childBuilder_.set(value, offset_);
    nulls_[offset_++] = false;
  }

  void add_null() {
    null_count_++;
    nulls_[offset_++] = true;
  }

  void step(c_type value) {
    childBuilder_.set(value, offset_);
    nulls_[offset_++] = false;
  }

  void skip() {
    null_count_++;
    nulls_[offset_++] = true;
  }

  void set(const c_type value, uint32_t offset) {
    childBuilder_.set(value, offset);
    nulls_[offset] = false;
  }



  int32_t length() {
    return length_;
  }

  const return_type* build() {
    return new return_type(*childBuilder_.build(), nulls_, null_count_);
  }

  const return_type* complete() {
    return build();
  }

private:
  ArrayBuilder<child_type> childBuilder_;
  int32_t length_;
  int32_t offset_;
  bool* nulls_;
  int32_t null_count_;
};

// special array builder for nullable primitive types
// THIS IS NOT NECESSARY but provides a little efficiency benefit
template<typename T>
class ArrayBuilder<Nullable<PrimitiveType<T> > >
{
public:
  using child_type = PrimitiveType<T>;
  using array_type = Nullable<child_type>;
  using return_type = Array<array_type>;
  using c_type = typename array_type::c_type;

  ArrayBuilder(int32_t length) : length_(length),
                                 offset_(0),
                                 values_(new c_type[length]),
                                 nulls_(new bool[length]),
                                 null_count_(0) {}

  void add(const c_type value) {
    values_[offset_] = value;
    nulls_[offset_++] = false;
  }

  void add_null() {
    null_count_++;
    nulls_[offset_++] = true;
  }

  void step(c_type value) {
    values_[offset_] = value;
    nulls_[offset_++] = false;
  }

  void skip() {
    null_count_++;
    nulls_[offset_++] = true;
  }

  void set(const c_type value, uint32_t offset) {
    values_[offset] = value;
    nulls_[offset] = false;
  }

  int32_t length() {
    return length_;
  }

  const return_type* build() {
    Array<child_type> *child_array = new Array<child_type>(values_, length_);
    return new return_type(*child_array, nulls_, null_count_);
  }

  const return_type* complete() {
    return build();
  }

private:
  c_type* values_;
  int32_t length_;
  int32_t offset_;
  bool* nulls_;
  int32_t null_count_;
};

} // namespace arrow


#endif