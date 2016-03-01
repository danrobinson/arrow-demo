#include <iostream>
#include <sstream>
#include "algorithms.h"
#include "take.h"
#include "take_alt.h"


using namespace arrow;

template<class T>
void printSum(T& array) {
  std::cout << "Sum of " + array.to_string() << std::endl;
  std::cout << Sum(array) << std::endl << std::endl;
}

template<class T>
void printMean(T& array) {
  std::cout << "Mean of " + array.to_string() << std::endl;
  std::cout << Mean(array) << std::endl << std::endl;
}

int main()
{
  typedef PrimitiveType<double> DoubleType;
  typedef PrimitiveType<uint32_t> UInt32Type;

  typedef Array<DoubleType> DoubleArray;
  typedef Array<Nullable<DoubleType> > NullableDoubleArray;

  typedef Array<UInt32Type> IntArray;
  typedef Array< Nullable<UInt32Type> > NullableIntArray;

  // see types.h for type definitions
  // see arrays.h for array templates
  double doubleValues[5] = {1.5, 2.5, 3.5, 5.5, 7.3};
  bool bitmask1[5] = {0, 1, 0, 1, 0};
  DoubleArray *doubArray = new DoubleArray(doubleValues, 5);
  NullableDoubleArray *nullableDoubArray = new NullableDoubleArray(doubArray, bitmask1);

  uint32_t intValues[3] = {0, 2, 3};
  bool bitmask2[3] = {1, 0, 0};
  IntArray *intArray = new IntArray(intValues, 3);
  NullableIntArray *nullableIntArray = new NullableIntArray(intArray, bitmask2);

  // see take.h for take implementation
  DoubleArray *result1 = Take(doubArray, intArray);
  NullableDoubleArray *result2 = Take(nullableDoubArray, intArray);
  NullableDoubleArray *result3 = Take(doubArray, nullableIntArray);
  NullableDoubleArray *result4 = Take(nullableDoubArray, nullableIntArray);



  std::cout << "Take using Nullable<T> template:" << std::endl;
  std::cout << result1->to_string() << std::endl;
  std::cout << result2->to_string() << std::endl;
  std::cout << result3->to_string() << std::endl;
  std::cout << result4->to_string() << std::endl;

  // ALTERNATE SYSTEM WITHOUT NULL TEMPLATES
  // see arrays_alt.h and builder_alt.h
  typedef ArrayAlt<UInt32Type> IntArrayAlt;
  typedef ArrayAlt<DoubleType> DoubleArrayAlt;
  bool nullFreeBitmask1[5] = {0, 0, 0, 0, 0};  
  bool nullFreeBitmask2[3] = {0, 0, 0};  

  DoubleArrayAlt *doubleArrayAlt = new DoubleArrayAlt(doubleValues, nullFreeBitmask1, 5);
  DoubleArrayAlt *doubleArrayAltWithNulls = new DoubleArrayAlt(doubleValues, bitmask1, 5);
  IntArrayAlt *intArrayAlt = new IntArrayAlt(intValues, nullFreeBitmask2, 3);
  IntArrayAlt *intArrayAltWithNulls = new IntArrayAlt(intValues, bitmask2, 3);

  // see take_alt.h for take implementation
  DoubleArrayAlt *result5 = TakeAlt(doubleArrayAlt, intArrayAlt);
  DoubleArrayAlt *result6 = TakeAlt(doubleArrayAltWithNulls, intArrayAlt);
  DoubleArrayAlt *result7 = TakeAlt(doubleArrayAlt, intArrayAltWithNulls);
  DoubleArrayAlt *result8 = TakeAlt(doubleArrayAltWithNulls, intArrayAltWithNulls);

  std::cout << "Take with alternate implementation:" << std::endl;
  std::cout << result5->to_string() << std::endl;
  std::cout << result6->to_string() << std::endl;
  std::cout << result7->to_string() << std::endl;
  std::cout << result8->to_string() << std::endl;

  // printSum(doubArray);
  // printSum(intArray);
  // printSum(nullableDoubArray);
  // printSum(nullableIntArray);
  // printMean(doubArray);
  // printMean(intArray);
  // printMean(nullableDoubArray);
  // printMean(nullableIntArray);
}