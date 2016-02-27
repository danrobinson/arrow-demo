#include "builder_alt.h"

namespace arrow {

// monolithic function
template<typename T, typename U>
ArrayAlt<T>* TakeAlt(ArrayAlt<T>* arr, ArrayAlt<U>* indexArr) {
  int32_t length = indexArr->length();
  ArrayBuilderAlt<T>* builder = new ArrayBuilderAlt<T>(length);
  if (arr->null_count()==0) {
    if (indexArr->null_count()==0) {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        builder->add(arr->get(indexArr->get(slotNumber)));
      }
    } else {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        if (indexArr->is_null(slotNumber)) {
          builder->add_null();
        } else {
          builder->add(arr->get(indexArr->get(slotNumber)));          
        }
      }
    }
  } else {
    int32_t index;
    if (indexArr->null_count()==0) {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        index = indexArr->get(slotNumber);
        if (arr->is_null(index)) {
          builder->add_null();
        } else {
          builder->add(arr->get(indexArr->get(index)));
        }
      }
    } else {
      for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
        if (indexArr->is_null(slotNumber)) {
          builder->add_null();
        }
        else {
          index = indexArr->get(slotNumber);
          if (arr->is_null(index)) {
            builder->add_null();
          } else {
            builder->add(arr->get(indexArr->get(index)));
          }
        }
      }
    }
  }
  return builder->build();
};


} // namespace arrow
