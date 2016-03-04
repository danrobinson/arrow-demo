#include "arrays.h"
#include "operations.h"

namespace arrow {

// these two versions of for_each could be combined into one function that uses a getOperation 
// but this is a little faster and doesn't make client-facing code any more complex
template<typename ArrayType, typename OperationType>
void for_each(const Array<ArrayType>& array, OperationType operation) {
  int32_t length = array.length();
  for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
    operation.step(array.get(slotNumber));
  }
}

template<typename ArrayType, typename OperationType>
void for_each(const Array<Nullable<ArrayType >>& array, OperationType operation) {
  int32_t length = array.length();
  for (int32_t slotNumber = 0; slotNumber < length; slotNumber++) {
    (array.is_null(slotNumber)) ? operation.skip() : operation.step(array.get(slotNumber));
  }
}

}