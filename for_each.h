

// these two versions of for_each could be combined into one function that uses a getOperation 
// but this is a little faster and doesn't make client-facing code any more complex
template<typename ArrayType, typename OperationType>
void for_each(const Array<ArrayType>& array, OperationType& operation) {
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

