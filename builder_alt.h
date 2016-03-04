#ifndef BUILDER_ALT_H
#define BUILDER_ALT_H

#include "arrays_alt.h"

namespace arrow {

template<typename T>
class ArrayBuilderAlt {};

template<typename T>
class ArrayBuilderAlt<PrimitiveType<T> >
{
public:
  typedef typename PrimitiveType<T>::c_type c_type;

  ArrayBuilderAlt(const int32_t length) : length_(length),
                                          offset_(0),
                                          nulls_(new bool[length]),
                                          values_(new c_type[length]),
                                          null_count_(0) {}

  void add(const c_type value) {
    values_[offset_] = value;
    nulls_[offset_++] = false;
  }

  void add_null() {
    null_count_++;
    nulls_[offset_++] = true;
  }

  int32_t length() {
    return length_;
  }

  const ArrayAlt<PrimitiveType<T> >* build() {
    return new ArrayAlt<PrimitiveType<T> >(values_, nulls_, length_, null_count_);
  }
private:
  const int32_t length_;
  int32_t offset_;
  int32_t null_count_;
  bool* nulls_;
  c_type* values_;
};

} // namespace arrow

#endif