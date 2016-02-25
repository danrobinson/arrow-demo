#include <iostream>
#include <sstream>
#include "algorithms.h"

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
  double doubleValues[3] = {1.5, 2.5, 3.5};
  uint32_t intValues[3] = {5, 6, 7};
  bool bitmask1[3] = {0, 1, 0};
  bool bitmask2[3] = {1, 0, 0};

  // see types.h for type definitions
  // see arrays.h for array templates
  Array<DoubleType> array1 (doubleValues, 3);
  Array<UInt32Type> array2 (intValues, 3);
  Array< Nullable<DoubleType> > nullableArray1 (&array1, bitmask1);
  Array< Nullable<UInt32Type> > nullableArray2 (&array2, bitmask2);

  printSum(array1);
  printSum(array2);
  printSum(nullableArray1);
  printSum(nullableArray2);
  printMean(array1);
  printMean(array2);
  printMean(nullableArray1);
  printMean(nullableArray2);
}