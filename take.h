#include "builder.h"
#include "apply.h" // foreach, get_from, append_to, and skip

namespace arrow {

// implementation of algorithm
template<typename T, typename U, typename V>
class TakeAlgorithm {
public:
  explicit TakeAlgorithm(T* arr, U* indexArr, V* builder) {
     // algorithm expressed in continuation-passing style
     for_each(indexArr, 
       get_from(arr, 
       append_to(builder)));
  }
};

// trick for getting the return type of Take
// return type is a nullable version of T iff either T or U is nullable
template<typename T, typename U>
using TakeReturnType = typename std::conditional<is_nullable<T>::value || !is_nullable<U>::value, T, Nullable<T> >::type;

// Take function called by user
template<typename T, typename U, typename V = TakeReturnType<T, U> >
Array<V>* Take(Array<T>* arr, Array<U>* indexArr) {
  auto *builder = new ArrayBuilder<V>(indexArr->length());
  Skip2<TakeAlgorithm>(arr, indexArr, builder);
  return builder->build();
}




} // namespace arrow
