#import "arrays.h"

namespace arrow {

// helper template handling the null logic for functions that take two arrays, 
// either or both of which could be nullable
// returns an array with the same type as the first array that is nullable if either or both arrays are nullable
template<template <typename, typename, typename> class Functor, typename T, typename U>
Array<T>* ArrayCombiner(Array<T>* arr, Array<U>* secondArr) {
  return Functor<T, T, U >() (arr, secondArr);    
};

template<template <typename, typename, typename> class Functor, typename T, typename U>
Array<Nullable<T> >* ArrayCombiner(Array<Nullable<T> >* arr, Array<U>* secondArr) {
  if (arr->null_count() == 0) {
    return Functor<Nullable<T>, T, U >() (arr->child_array(), secondArr);        
  } else {
    return Functor<Nullable<T>, Nullable<T>, U >() (arr, secondArr);
  }
};

template<template <typename, typename, typename> class Functor, typename T, typename U>
Array<Nullable<T> >* ArrayCombiner(Array<T>* arr, Array<Nullable<U> >* secondArr) {
  if (secondArr->null_count() == 0) {
    return Functor<Nullable<T>, T, U >() (arr, secondArr->child_array());    
  } else {
    return Functor<Nullable<T>, T, Nullable<U> >() (arr, secondArr);
  }
};

template<template <typename, typename, typename> class Functor, typename T, typename U>
Array<Nullable<T> >* ArrayCombiner(Array<Nullable<T> >* arr, Array<Nullable<U> >* secondArr) {
  if (arr->null_count() == 0) {
    if (secondArr->null_count() == 0) {
      return Functor<Nullable<T>, T, U >() (arr->child_array(), secondArr->child_array());         
    } else {
      return Functor<Nullable<T>, T, Nullable<U> >() (arr->child_array(), secondArr);         
    }
  } else {
    if (secondArr->null_count() == 0) {
      return Functor<Nullable<T>, Nullable<T>, U >() (arr, secondArr->child_array());         
    } else {
      return Functor<Nullable<T>, Nullable<T>, Nullable<U> >() (arr, secondArr);         
    }
  }
};



// simple hack for getting the return type
template<typename T, typename U>
struct GetReturnType {
public:
  typedef T result;
};

template<typename T, typename U>
struct GetReturnType<Nullable<T>, U> {
public:
  typedef Nullable<T> result;
};

template<typename T, typename U>
struct GetReturnType<T, Nullable<U> > {
public:
  typedef Nullable<T> result;
};

template<typename T, typename U>
struct GetReturnType<Nullable<T>, Nullable<U> > {
public:
  typedef Nullable<T> result;
};


} // namespace arrow
