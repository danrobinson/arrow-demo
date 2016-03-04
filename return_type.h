#ifndef RETURN_TYPE_H
#define RETURN_TYPE_H

#include "operations.h"

namespace arrow {

// logic for figuring out the return type of strings of operations

template<typename operation_type, typename previous_return_type, typename next_op_type>
class ReturnType {};

template<typename next_op_type, typename this_return_type>
using GetReturnType = typename ReturnType<next_op_type, this_return_type, typename next_op_type::next_op_type>::type;

template<typename T, typename previous_return_type, typename next_op_type>
class ReturnType<GetOperation<const Array<T>, next_op_type>, previous_return_type, next_op_type> {
public:
  using type = GetReturnType<next_op_type, propagate_null<previous_return_type, T> >;
};

} // namespace arrow

#endif