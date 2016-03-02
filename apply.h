#include "builder.h"
#include "timer.h"

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

template<typename ArrayType, typename NextOperationType>
class GetOperation : public Operation<NextOperationType> {
public:
  using next_operation_type = NextOperationType;
  explicit GetOperation(const ArrayType& array, NextOperationType& next) : array_(array),
                                                                              Operation<NextOperationType>(next) {}

  void step(const uint32_t slotNumber) {
    this->next_.step(array_.get(slotNumber));
  }

private:
  const ArrayType array_;
};

template<typename ArrayType, typename NextOperationType>
class GetOperation<const Array<Nullable<ArrayType> >, NextOperationType> : public Operation<NextOperationType> {
public:
  using next_operation_type = NextOperationType;
  explicit GetOperation(const Array<Nullable<ArrayType> >& array, NextOperationType& next) : array_(array), 
                                                                                    Operation<NextOperationType>(next) {}

  void step(const uint32_t slotNumber) {
    (array_.is_null(slotNumber)) ? this->next_.skip() : this->next_.step(array_.get(slotNumber));
  }

private:
  const Array<Nullable<ArrayType> > array_;
};

class BaseType {
public:
  using type = void;
  using next_operation_type = void;
};

template<typename operation_type, typename previous_return_type, typename... other>
class ReturnType {};

template<typename T, typename previous_return_type, typename next_operation_type>
class ReturnType<GetOperation<Array<T>, next_operation_type>, previous_return_type, next_operation_type> {
public:
  using this_return_type = propagate_null<previous_return_type, T>;
  using type = typename ReturnType<next_operation_type, this_return_type, typename next_operation_type::next_operation_type>::type;
};

template<typename previous_return_type, typename next_operation_type>
class ReturnType<BaseType, previous_return_type, next_operation_type> {
public:
  using type = previous_return_type;
};


template<typename ArrayType, typename OperationType>
GetOperation<ArrayType, OperationType>& get_from(const ArrayType& array, OperationType& next) {
  auto *getOp = new GetOperation<ArrayType, OperationType>(array, next);
  return *getOp;
}


// these two versions of for_each could be combined into one function that uses a getOperation 
// but this is a little faster and doesn't make client-facing code any more complex
template<typename ArrayType, typename OperationType>
void for_each(const ArrayType& array, OperationType& operation) {
  int32_t length = array.length();
  for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
    operation.step(array.get(slotNumber));
  }
}

template<typename ArrayType, typename OperationType>
void for_each(const Array<Nullable<ArrayType >>& array, OperationType& operation) {
  int32_t length = array.length();
  for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
    (array.is_null(slotNumber)) ? operation.skip() : operation.step(array.get(slotNumber));
  }
}

// the array builders already define step() and skip(), so there's no need to add an operation here
// this just makes algorithms read a little more naturally
template<typename ArrayType>
ArrayBuilder<ArrayType>& append_to(ArrayBuilder<ArrayType>& builder) {
  return builder;
}

template<template<typename, typename> class operation_type, typename argument_type, typename next_operation_type, 
                    typename new_operation_type = operation_type<argument_type, next_operation_type> >
new_operation_type& apply(argument_type& arg, next_operation_type& next_operation) {
  auto *op = new new_operation_type(arg, next_operation);
  return *op;
}

template<template<typename, typename> class operation_type, typename array_type, typename argument_type>
using MapReturnType = typename ReturnType<operation_type<argument_type, BaseType>, array_type, BaseType>::type;

template<template<typename, typename> class operation_type, typename array_type, typename argument_type,
                    typename return_type = MapReturnType<operation_type, array_type, argument_type> >
const Array<return_type>* map(const Array<array_type>& arr, const argument_type& arg) {
  ArrayBuilder<return_type> builder(arr.length());
  for_each(arr, 
    apply<operation_type>(arg,
    append_to(builder)));
  return builder.build();
}




} // namespace arrow