#include "builder.h"
#import "null_logic.h"


namespace arrow {

// helper functions: one for nullable indexArray, one for non-nullable indexArray
template<typename ReturnType, typename T, typename U>
void BuildFromIndex(ArrayBuilder<ReturnType>* builder, Array<T>* arr, Array<U>* indexArray, int32_t slotNumber) {
  builder->copyAtIndex(arr, (int32_t)indexArray->get(slotNumber));
};

template<typename ReturnType, typename T, typename U>
void BuildFromIndex(ArrayBuilder<ReturnType>* builder, Array<T>* arr, Array<Nullable<U> >* indexArray, int32_t slotNumber) {
  if (indexArray->is_null(slotNumber)) {
    builder->add_null();
  } else {
    builder->copyAtIndex(arr, (int32_t)indexArray->child_array()->get(slotNumber));  
  }
};

// "polymorphic" function with most of the Take logic
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
template<typename T, typename U>
Array<typename GetReturnType<T, U>::result>* Take(Array<T>* arr, Array<U>* indexArr) {
  return ArrayCombiner<TakeFunctor>(arr, indexArr);
}

} // namespace arrow
