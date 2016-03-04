#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "builder.h"

namespace arrow {

template<typename OperationType>
class Operation {
public:
  explicit Operation(OperationType& next) : next_(next) {}

  // passes null results through
  void skip() {
    next_.skip();
  }

protected:
  OperationType next_;
};

// get operation, initialized with an array
// at each step, is passed an index by the previous operation
// it gets the value at that index and passes it to the next operation
// two versions: one for nullable arrays, one for non-nullable arrays

template<typename array_type, typename next_operation_type>
class GetOperation : public Operation<next_operation_type> {
public:
  using next_op_type = next_operation_type;
  explicit GetOperation(array_type& array, next_operation_type& next) : array_(array),
                                                                        Operation<next_operation_type>(next) {}

  void step(const uint32_t slotNumber) {
    this->next_.step(array_.get(slotNumber));
  }

private:
  array_type array_;
};

template<typename T, typename next_operation_type>
class GetOperation<const Array<Nullable<T> >, next_operation_type> : public Operation<next_operation_type> {
public:
  using next_op_type = next_operation_type;
  explicit GetOperation(const Array<Nullable<T> >& array, next_operation_type& next) : array_(array), 
                                                                                    Operation<next_operation_type>(next) {}

  void step(const uint32_t slotNumber) {
    (array_.is_null(slotNumber)) ? this->next_.skip() : this->next_.step(array_.get(slotNumber));
  }

private:
  const Array<Nullable<T> > array_;
};

template<typename ArrayType, typename OperationType>
GetOperation<ArrayType, OperationType> get_from(const ArrayType& array, OperationType next) {
  return GetOperation<ArrayType, OperationType>(array, next);
}

// the array builders already define step() and skip(), so there's no need to add an operation here
// this just makes algorithms read a little more naturally
template<typename ArrayType>
ArrayBuilder<ArrayType> append_to(ArrayBuilder<ArrayType>& builder) {
  return builder;
}

template<template<typename, typename> class operation_type, typename argument_type, typename next_operation_type, 
                    typename new_operation_type = operation_type<argument_type, next_operation_type> >
new_operation_type apply(argument_type& arg, next_operation_type next_operation) {
  return new_operation_type(arg, next_operation);
}

} // namespace arrow


#endif