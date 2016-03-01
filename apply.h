#include "builder.h"
#include "operation.h"

namespace arrow {

template<typename ArrayType, typename OperationType>
class GetOperation : public Operation<OperationType> {
public:
  explicit GetOperation(const Array<ArrayType>& array, OperationType& next) : array_(array),
                                                                              Operation<OperationType>(next) {}

  void step(const uint32_t slotNumber) {
    this->next_.step(array_.get(slotNumber));
  }

private:
  const Array<ArrayType> array_;
};

template<typename ArrayType, typename OperationType>
class GetOperation<Nullable<ArrayType>, OperationType> : public Operation<OperationType> {
public:
  explicit GetOperation(const Array<Nullable<ArrayType > >& array, OperationType& next) : array_(array), 
                                                                                    Operation<OperationType>(next) {}

  void step(const uint32_t slotNumber) {
    (array_.is_null(slotNumber)) ? this->next_.skip() : this->next_.step(array_.get(slotNumber));
  }

private:
  const Array<Nullable<ArrayType> > array_;
};

template<typename ArrayType, typename OperationType>
void for_each(const Array<ArrayType>& array, OperationType& operation) {
  int32_t length = array.length();
  GetOperation<ArrayType, OperationType> get_op(array, operation);
  for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
    get_op.step(slotNumber);
  }
}

template<typename ArrayType, typename OperationType>
GetOperation<ArrayType, OperationType>& get_from(const Array<ArrayType>& array, OperationType& next) {
  auto *getOp = new GetOperation<ArrayType, OperationType>(array, next);
  return *getOp;
}

// the array builders already define step() and skip(), so there's no need to add an operation here
// this just makes algorithms read a little more naturally
template<typename ArrayType>
ArrayBuilder<ArrayType>& append_to(ArrayBuilder<ArrayType>& builder) {
  return builder;
}

// helpers for skipping 
template<template <typename... > class AlgorithmTemplate, typename T, typename... OtherArgs>
void Skip(const Array<T>& arr, OtherArgs& ...args) {
  AlgorithmTemplate<Array<T>, OtherArgs...>(arr, args...);
}

template<template <typename... > class AlgorithmTemplate, typename T, typename... OtherArgs>
void Skip(const Array<Nullable<T> >& arr, OtherArgs& ...args) {
  if (arr.no_nulls()) {
    AlgorithmTemplate<Array<T>, OtherArgs...>(arr.child_array(), args...);
  } else {
    AlgorithmTemplate<Array<Nullable<T> >, OtherArgs...>(arr, args...);
  }
}

template<template <typename... > class AlgorithmTemplate, typename T, typename U, typename... OtherArgs>
void Skip2(const Array<T>& arr, const Array<U>& second_arr, OtherArgs& ...args) {
  Skip<AlgorithmTemplate>(arr, second_arr, args...);
}

template<template <typename... > class AlgorithmTemplate, typename T, typename U, typename... OtherArgs>
void Skip2(const Array<T>& arr, const Array<Nullable<U> >& second_arr, OtherArgs& ...args) {
  if (second_arr.no_nulls()) {
    Skip<AlgorithmTemplate>(arr, second_arr.child_array(), args...);
  } else {
    Skip<AlgorithmTemplate>(arr, second_arr, args...);
  }
}


} // namespace arrow