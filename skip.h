#import "arrays.h"

namespace arrow {

// helpers for skipping null-handling logic for "nullable" arrays that contain no nulls

template<template <typename... > class AlgorithmTemplate, typename return_type, typename T, typename... OtherArgs>
return_type* Skip(const Array<T>& arr, OtherArgs& ...args) {
  return AlgorithmTemplate<return_type, Array<T>, OtherArgs...>() (arr, args...);
}

template<template <typename... > class AlgorithmTemplate, typename return_type, typename T, typename... OtherArgs>
return_type* Skip(const Array<Nullable<T> >& arr, OtherArgs& ...args) {
  if (arr.no_nulls()) {
    return AlgorithmTemplate<return_type, Array<T>, OtherArgs...>() (arr.child_array(), args...);
  } else {
    return AlgorithmTemplate<return_type, Array<Nullable<T> >, OtherArgs...>() (arr, args...);
  }
}

template<template <typename... > class AlgorithmTemplate, typename return_type, typename T, typename U, typename... OtherArgs>
return_type* Skip2(const Array<T>& arr, const Array<U>& second_arr, OtherArgs& ...args) {
  return Skip<AlgorithmTemplate, return_type>(arr, second_arr, args...);
}

template<template <typename... > class AlgorithmTemplate, typename return_type, typename T, typename U, typename... OtherArgs>
return_type* Skip2(const Array<T>& arr, const Array<Nullable<U> >& second_arr, OtherArgs& ...args) {
  if (second_arr.no_nulls()) {
    return Skip<AlgorithmTemplate, return_type>(arr, second_arr.child_array(), args...);
  } else {
    return Skip<AlgorithmTemplate, return_type>(arr, second_arr, args...);
  }
}

}