#ifndef BUILDER_H_
#define BUILDER_H_

#include "arrays.h"

namespace arrow {

template<typename T>
class ArrayBuilder
{
public:
  typedef typename T::value_type value_type;
  typedef ArrayBuilder<T> BuilderType;

  ArrayBuilder(int32_t length) : length_(length) {
    offset_ = 0;
    values_ = new value_type[length];
  }

  void add(const value_type value) {
    values_[offset_++] = value;
  }

  const int32_t length() { return length_; }

  void increment_offset() {
    offset_++;
  }

  Array<T>* build() {
    return new Array<T>(values_, length_);
  }

  void step(const value_type value) {
    add(value);
  }

private:
  const int32_t length_;
  int32_t offset_;
  value_type* values_;
};

template<typename T>
class ArrayBuilder<Nullable<T> >
{
public:
  typedef typename T::value_type value_type;
  typedef ArrayBuilder<Nullable<T> > BuilderType;

  ArrayBuilder(int32_t length) : length_(length),
                                 offset_(0),
                                 childBuilder_(length),
                                 nulls_(new bool[length]),
                                 null_count_(0) {}

  void add(const value_type value) {
    nulls_[offset_++] = false;
    childBuilder_.add(value);
  }

  void add_null() {
    nulls_[offset_++] = true;
    null_count_++;
    childBuilder_.increment_offset();
  }

  int32_t length() {
    return length_;
  }

  void increment_offset() {
    offset_++;
  }

  Array<Nullable<T> >* build() {
    return new Array<Nullable<T> >(*childBuilder_.build(), nulls_, null_count_);
  }

  void step(value_type value) {
    add(value);
  }

  void skip() {
    add_null();
  }

private:
  ArrayBuilder<T> childBuilder_;
  int32_t length_;
  int32_t offset_;
  bool* nulls_;
  int32_t null_count_;
};

} // namespace arrow

#endif