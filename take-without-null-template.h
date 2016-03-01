#include "builder.h"

namespace arrow {

// helper functions: one for nullable indexArray, one for non-nullable indexArray
// 4 LoC
template<typename T, typename U, typename V>
void BuildFromIndex(ArrayBuilder<T>* builder, Array<U>* arr, Array<V>* indexArray, int32_t slotNumber) {
  builder->copyAtIndex(arr, (int32_t)indexArray->get(slotNumber));
};

// 9 LoC
template<typename T, typename U, typename V>
void BuildFromIndex(ArrayBuilder<T>* builder, Array<U>* arr, Array<Nullable<V> >* indexArray, int32_t slotNumber) {
  if (indexArray->is_null(slotNumber)) {
    builder->add_null();
  } else {
    builder->copyAtIndex(arr, (int32_t)indexArray->child_array()->get(slotNumber));  
  }
};

// "polymorphic" function with most of the Take logic
// 12 LoC
template<typename ReturnType, typename T, typename U>
class TakeFunctor {
public:
  Array<ReturnType>* operator() (Array<T>* arr, Array<U>* indexArr) {
    int32_t length = indexArr->length();
    ArrayBuilder<ReturnType>* builder = new ArrayBuilder<ReturnType>(length);
    for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
      BuildFromIndex(builder, arr, indexArr, slotNumber);
    }
    return builder->build();
  };
};

template<typename ReturnType, typename T, typename U>
class TakeFunctor {
public:
  Array<ReturnType>* operator() (Array<T>* arr, Array<U>* indexArr) {
    int32_t length = indexArr->length();
    ArrayBuilder<ReturnType>* builder = new ArrayBuilder<ReturnType>(length);
    for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
      BuildFromIndex(builder, arr, indexArr, slotNumber);
    }
    return builder->build();
  };
};

// use ArrayCombiner template to handle the 4 cases
// 4 LoC
template<typename T, typename U>
Array<typename GetReturnType<T, U>::result>* Take(Array<T>* arr, Array<U>* indexArr) {
  return ArrayCombiner<TakeFunctor>(arr, indexArr);
}

} // namespace arrow
