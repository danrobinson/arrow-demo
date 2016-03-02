#include <iostream>
#include <sstream>
#include "take.h"
#include "timer.h"
#include "take_alt.h" // alternate implementation of take

using namespace arrow;

#define ARRAY_LEN 10
#define INT_ARRAY_LEN 5
#define REPS 1

typedef std::chrono::steady_clock::time_point TimePoint;

// see types.h for type definitions
typedef PrimitiveType<double> DoubleType;
typedef PrimitiveType<uint32_t> UInt32Type;

// see arrays.h for array templates
typedef Array<DoubleType> DoubleArray;
typedef Array<Nullable<DoubleType> > NullableDoubleArray;
typedef Array<UInt32Type> IntArray;
typedef Array< Nullable<UInt32Type> > NullableIntArray;

// see arrays_alt.h and builder_alt.h
typedef ArrayAlt<UInt32Type> IntArrayAlt;
typedef ArrayAlt<DoubleType> DoubleArrayAlt;

double randDouble()
{
    return (double)rand() / RAND_MAX;
}

// random boolean, 50% true, 50% false
bool randBool()
{
    return (rand() % 2 == 0);
}

uint32_t randInt() 
{
  return rand() % ARRAY_LEN;
}

int main()
{
  srand(time(0));

  // raw values for double array
  double* doubleValues = new double[ARRAY_LEN];
  bool* bitmask1 = new bool[ARRAY_LEN];
  bool* nullFreeBitmask1 = new bool[ARRAY_LEN];
  int32_t nullCount1 = 0;
  bool is_null;
  for (int32_t i = 0; i < ARRAY_LEN; i++) {
    doubleValues[i] = randDouble();
    is_null = randBool();
    bitmask1[i] = is_null;
    nullCount1 += is_null;
    nullFreeBitmask1[i] = false;
  }

  // raw values for integer array
  uint32_t* intValues = new uint32_t[INT_ARRAY_LEN];
  bool* bitmask2 = new bool[INT_ARRAY_LEN];
  bool* nullFreeBitmask2 = new bool[INT_ARRAY_LEN];
  int32_t nullCount2 = 0;
  for (int32_t i = 0; i < INT_ARRAY_LEN; i++) {
    intValues[i] = randInt();
    is_null = randBool();
    bitmask2[i] = is_null;
    nullCount2 += is_null;
    nullFreeBitmask2[i] = false;
  }

  // arrays
  const DoubleArray doubArray(doubleValues, ARRAY_LEN);
  const NullableDoubleArray nullableDoubArray(doubArray, bitmask1, nullCount1);
  const IntArray intArray(intValues, INT_ARRAY_LEN);
  const NullableIntArray nullableIntArray(intArray, bitmask2, nullCount2);

  const DoubleArrayAlt doubleArrayAlt(doubleValues, nullFreeBitmask1, ARRAY_LEN);
  const DoubleArrayAlt doubleArrayAltWithNulls(doubleValues, bitmask1, ARRAY_LEN, nullCount1);
  const IntArrayAlt intArrayAlt(intValues, nullFreeBitmask2, INT_ARRAY_LEN);
  const IntArrayAlt intArrayAltWithNulls (intValues, bitmask2, INT_ARRAY_LEN, nullCount2);

  // timer.reset();
  // for (int32_t i = 0; i < REPS; i++) {
  auto *result1 = TakeAlt(doubleArrayAlt, intArrayAlt);
  auto *result2 = TakeAlt(doubleArrayAltWithNulls, intArrayAlt);
  auto *result3 = TakeAlt(doubleArrayAlt, intArrayAltWithNulls);
  auto *result4 = TakeAlt(doubleArrayAltWithNulls, intArrayAltWithNulls);
  // }
  // std::cout << "Alternate Results" << std::endl;
  // std::cout << timer.to_string() << std::endl;

  // timer.reset();
  // for (int32_t i = 0; i < REPS; i++) {
  auto *result5 = Take(intArray, doubArray);
  auto *result6 = Take(intArray, nullableDoubArray);
  auto *result7 = Take(nullableIntArray, doubArray);
  auto *result8 = Take(nullableIntArray, nullableDoubArray);
  // }
  // std::cout << "Template Test Results" << std::endl;
  // std::cout << timer.to_string() << std::endl;

  std::cout << result1->to_string() << std::endl;
  std::cout << result5->to_string() << std::endl;
  std::cout << result2->to_string() << std::endl;
  std::cout << result6->to_string() << std::endl;
  std::cout << result3->to_string() << std::endl;
  std::cout << result7->to_string() << std::endl;
  std::cout << result4->to_string() << std::endl;
  std::cout << result8->to_string() << std::endl;

  delete[] doubleValues;
  delete[] bitmask1;
  delete[] nullFreeBitmask1;
  delete[] intValues;
  delete[] bitmask2;
  delete[] nullFreeBitmask2;
}