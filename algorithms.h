#include "arrays.h"

namespace arrow {

// Sum

template<typename T>
typename T::value_type CalculateSum(Array<T>& arr) {
  typename T::value_type result = 0;
  int32_t length = arr.length();
  for (int32_t i = 0; i < length; i++) {
    result += arr.get(i, 0);
  }
  return result;
}

template<typename T>
typename T::value_type Sum(Array<T>& arr) {
  return CalculateSum(arr);
}

template<typename T>
typename T::value_type Sum(Array< Nullable<T> >& nullableArr) {
  return (nullableArr.null_count() == 0) ? Sum(*nullableArr.child_array()) : CalculateSum(nullableArr);
}

// Mean

template<typename T>
double Mean(Array<T>& arr) {
  return (double)Sum(arr) / (arr.length());
}

template<typename T>
double Mean(Array< Nullable<T> >& nullableArr) {
  return (double)Sum(nullableArr) / (nullableArr.length() - nullableArr.null_count());
}

} // namespace arrow