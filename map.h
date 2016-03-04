#include "builder.h"
#include "for_each.h"
#include "skip.h"
#include "operations.h"
#include "return_type.h"

namespace arrow {

// declarations of skipping helpers (see below)
template<template<typename, typename> class operation_type, typename return_type, typename T, typename U>
return_type* check_first(T& arg1, U& arg2);

template<template<typename, typename> class operation_type, typename return_type, typename T, typename U>
return_type* check_second(T& arg1, U& arg2);


template<template<typename, typename> class operation_type, typename return_type, typename array_type, typename argument_type>
return_type* calculate_map(array_type& arr, argument_type& arg) {
  // algorithm can be written in continuation-passing style
  ArrayBuilder<typename return_type::value_type> builder(arr.length());
  for_each(arr,
    apply<operation_type>(arg,
    append_to(builder)));
  return builder.build();
}

// helper for determining return type at runtime

class MapBuildType {
public:
  using type = void;
  using next_op_type = void;
};

template<typename previous_return_type, typename next_op_type>
class ReturnType<MapBuildType, previous_return_type, next_op_type> {
public:
  using type = const Array<previous_return_type>;
};

template<template<typename, typename> class operation_type, typename array_type, typename argument_type>
using MapReturnType = GetReturnType<operation_type<argument_type, MapBuildType>, typename array_type::value_type>;


// map function callable by user

template<template<typename, typename> class operation_type, typename array_type, typename argument_type,
                    typename return_type = MapReturnType<operation_type, array_type, argument_type> >
return_type* map(array_type& arr, argument_type& arg) {
  return check_first<operation_type, return_type>(arr, arg);
}


// logic for skipping null-handling logic for nullable arrays that have no null values actually in them

template<template<typename, typename> class operation_type, typename return_type, typename T, typename U>
return_type* check_first(T& arg1, U& arg2) {
  return check_second<operation_type, return_type>(arg1, arg2);
}

template<template<typename, typename> class operation_type, typename return_type, typename T, typename U>
return_type* check_first(const Array<Nullable<T> >& arr, U& arg2) {
  if (arr.no_nulls()) {
    return check_second<operation_type, return_type>(arr.child_array(), arg2);
  } else {
    return check_second<operation_type, return_type>(arr, arg2);
  }
}

template<template<typename, typename> class operation_type, typename return_type, typename T, typename U>
return_type* check_second(T& arg1, U& arg2) {
  return calculate_map<operation_type, return_type>(arg1, arg2);
}

template<template<typename, typename> class operation_type, typename return_type, typename T, typename U>
return_type* check_second(T& arg1, const Array<Nullable<U> >& arr) {
  if (arr.no_nulls()) {
    return calculate_map<operation_type, return_type>(arg1, arr.child_array());
  } else {
    return calculate_map<operation_type, return_type>(arg1, arr);
  }
}




}
