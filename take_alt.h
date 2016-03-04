#ifndef TAKE_ALT_H
#define TAKE_ALT_H
#include "builder_alt.h"

namespace arrow {

// monolithic function
template<typename T, typename U, typename V>
void CalculateTakeAlt(const T& arr, const U& indexArr, V builder) {
  const int32_t length = indexArr.length();
  if (arr.no_nulls()) {
    if (indexArr.no_nulls()) {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        builder.add(arr.get(indexArr.get(slotNumber)));
      }
    } else {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        if (indexArr.is_null(slotNumber)) {
          builder.add_null();
        } else {
          builder.add(arr.get(indexArr.get(slotNumber)));          
        }
      }
    }
  } else {
    int32_t index;
    if (indexArr.no_nulls()) {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        index = indexArr.get(slotNumber);
        if (arr.is_null(index)) {
          builder.add_null();
        } else {
          builder.add(arr.get(index));
        }
      }
    } else {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        if (indexArr.is_null(slotNumber)) {
          builder.add_null();
        }
        else {
          index = indexArr.get(slotNumber);
          if (arr.is_null(index)) {
            builder.add_null();
          } else {
            builder.add(arr.get(index));
          }
        }
      }
    }
  }
};

template<typename T, typename U>
const ArrayAlt<T> TakeAlt(const ArrayAlt<T>& arr, const ArrayAlt<U>& indexArr) {
  int32_t length = indexArr.length();
  ArrayBuilderAlt<T> builder(length);
  CalculateTakeAlt(arr, indexArr, builder);
  return builder.build();
}


} // namespace arrow

#endif
