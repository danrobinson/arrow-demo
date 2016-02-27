#include "arrays.h"

namespace arrow {

template<typename T>
class ArrayBuilder
{
public:
  typedef typename T::value_type value_type;

  ArrayBuilder(int32_t length) : length_(length) {
    offset_ = 0;
    values_ = new value_type[length];
  }

  void copyAtIndex(Array<T>* array, int32_t index) {
    add(array->get(index));
  }

  void add(value_type value) {
    values_[offset_++] = value;
  }

  int32_t length() { return length_; }

  void increment_offset() {
    offset_++;
  }

  Array<T>* build() {
    return new Array<T>(values_, length_);
  }

private:
  int32_t length_;
  int32_t offset_;
  value_type* values_;
};

template<typename T>
class ArrayBuilder<Nullable<T> >
{
public:
  typedef typename T::value_type value_type;

  ArrayBuilder(int32_t length) : length_(length),
                                 offset_(0),
                                 childBuilder_(new ArrayBuilder<T>(length)),
                                 nulls_(new bool[length]) {}

  void copyAtIndex(Array<T>* array, int32_t index) {
    add(array->get(index));
  }

  void copyAtIndex(Array<Nullable<T> >* array, int32_t index) {
    if (array->is_null(index)) {
      add_null();
    } else {
      increment_offset();
      childBuilder_->copyAtIndex(array->child_array(), index);
    }
  }

  void add(value_type value) {
    nulls_[offset_++] = false;
    childBuilder_->add(value);
  }

  void add_null() {
    nulls_[offset_++] = true;
    childBuilder_->increment_offset();
  }

  int32_t length() {
    return length_;
  }

  void increment_offset() {
    offset_++;
  }

  Array<Nullable<T> >* build() {
    return new Array<Nullable<T> >(childBuilder_->build(), nulls_);
  }
private:
  ArrayBuilder<T>* childBuilder_;
  int32_t length_;
  int32_t offset_;
  bool* nulls_;
};

} // namespace arrow