
template<typename ArrayType, typename IndexArrayType>
Array<ArrayType>* Take(Array<ArrayType>* arr, Array<IndexArrayType>* indexArr) {
  return CalculateTake<ArrayType>(arr, indexArr);    
};

template<typename ArrayType, typename IndexArrayType>
Array<Nullable<ArrayType> >* Take(Array<Nullable<ArrayType> >* arr, Array<IndexArrayType>* indexArr) {
  if (arr->null_count() == 0) {
    return CalculateTake<Nullable<ArrayType> >(arr->child_array(), indexArr);        
  } else {
    return CalculateTake<Nullable<ArrayType> >(arr, indexArr);
  }
};

template<typename ArrayType, typename IndexArrayType>
Array<Nullable<ArrayType> >* Take(Array<ArrayType>* arr, Array<Nullable<IndexArrayType> >* indexArr) {
  if (indexArr->null_count() == 0) {
    return CalculateTake<Nullable<ArrayType> >(arr, indexArr->child_array());    
  } else {
    return CalculateTake<Nullable<ArrayType> >(arr, indexArr);
  }
};

template<typename ArrayType, typename IndexArrayType>
Array<Nullable<ArrayType> >* Take(Array<Nullable<ArrayType> >* arr, Array<Nullable<IndexArrayType> >* indexArr) {
  if (arr->null_count() == 0) {
    if (indexArr->null_count() == 0) {
      return CalculateTake<Nullable<ArrayType> >(arr->child_array(), indexArr->child_array());         
    } else {
      return CalculateTake<Nullable<ArrayType> >(arr->child_array(), indexArr);         
    }
  } else {
    if (indexArr->null_count() == 0) {
      return CalculateTake<Nullable<ArrayType> >(arr, indexArr->child_array());         
    } else {
      return CalculateTake<Nullable<ArrayType> >(arr, indexArr);         
    }
  }
};
