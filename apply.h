#include "builder.h"
#include "operation.h"

namespace arrow {

template<typename ArrayType, typename OperationType>
class GetOperation : public Operation<OperationType> {
public:
  explicit GetOperation(Array<ArrayType>* array, OperationType* next) : array_(array), Operation<OperationType>(next) {}

  void step(uint32_t slotNumber) {
    this->next_->step(array_->get(slotNumber));
  }

private:
  Array<ArrayType>* array_;
};

template<typename ArrayType, typename OperationType>
class GetOperation<Nullable<ArrayType>, OperationType> : public Operation<OperationType> {
public:
  explicit GetOperation(Array<Nullable<ArrayType > >* array, OperationType* next) : array_(array), Operation<OperationType>(next) {}

  void step(uint32_t slotNumber) {
    if (array_->is_null(slotNumber)) {
      this->next_->skip();
    } else {
      this->next_->step(array_->get(slotNumber));
    }
  }

private:
  Array<Nullable<ArrayType> >* array_;
};

template<typename ArrayType, typename OperationType>
void for_each(Array<ArrayType>* array, OperationType* operation) {
  int32_t length = array->length();
  GetOperation<ArrayType, OperationType> *get_op = get_from(array, operation);
  for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
    get_op->step(slotNumber);
  }
}

template<typename ArrayType, typename OperationType>
GetOperation<ArrayType, OperationType>* get_from(Array<ArrayType>* array, OperationType* next) {
  return new GetOperation<ArrayType, OperationType>(array, next);
}

// the array builders already define step() and skip(), so there's no need to add an operation here
// this just makes algorithms read a little more naturally
template<typename ArrayType>
ArrayBuilder<ArrayType>* append_to(ArrayBuilder<ArrayType>* builder) {
  return builder;
}

// helpers for skipping 
template<template <typename... > class AlgorithmTemplate, typename T, typename... OtherArgs>
void Skip(Array<T> *arr, OtherArgs* ...args) {
  AlgorithmTemplate<Array<T>, OtherArgs...>(arr, args...);
}

template<template <typename... > class AlgorithmTemplate, typename T, typename... OtherArgs>
void Skip(Array<Nullable<T> > *arr, OtherArgs* ...args) {
  if (arr->no_nulls()) {
    AlgorithmTemplate<Array<T>, OtherArgs...>(arr->child_array(), args...);
  } else {
    AlgorithmTemplate<Array<Nullable<T> >, OtherArgs...>(arr, args...);
  }
}

template<template <typename... > class AlgorithmTemplate, typename T, typename U, typename... OtherArgs>
void Skip2(Array<T> *arr, Array<U>* second_arr, OtherArgs* ...args) {
  Skip<AlgorithmTemplate>(arr, second_arr, args...);
}

template<template <typename... > class AlgorithmTemplate, typename T, typename U, typename... OtherArgs>
void Skip2(Array<T> *arr, Array<Nullable<U> >* second_arr, OtherArgs* ...args) {
  if (second_arr->no_nulls()) {
    Skip<AlgorithmTemplate>(arr, second_arr->child_array(), args...);
  } else {
    Skip<AlgorithmTemplate>(arr, second_arr, args...);
  }
}


} // namespace arrow