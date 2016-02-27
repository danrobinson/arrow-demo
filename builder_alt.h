#include "arrays_alt.h"

namespace arrow {

template<typename T>
class ArrayBuilderAlt
{
public:
  typedef typename T::value_type value_type;

  ArrayBuilderAlt(int32_t length) : length_(length),
                                    offset_(0),
                                    nulls_(new bool[length]),
                                    values_(new value_type[length]) {}
  void add(value_type value) {
    values_[offset_] = value;
    nulls_[offset_++] = false;
  }

  void add_null() {
    nulls_[offset_++] = true;
  }

  int32_t length() {
    return length_;
  }

  void increment_offset() {
    offset_++;
  }

  ArrayAlt<T>* build() {
    return new ArrayAlt<T>(values_, nulls_, length_);
  }
private:
  int32_t length_;
  int32_t offset_;
  bool* nulls_;
  value_type* values_;
};

} // namespace arrow