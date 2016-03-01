#include <iostream>
#include <sstream>
#include "take.h"
#include "take_alt.h" // alternate implementation of take
#include "timer.h"



using namespace arrow;

#define ARRAY_LEN 1000000
#define INT_ARRAY_LEN 1000000
#define REPS 10

// see types.h for type definitions
typedef PrimitiveType<double> DoubleType;
typedef PrimitiveType<uint32_t> UInt32Type;

// see arrays.h for array templates
typedef Array<DoubleType> DoubleArray;
typedef Array<Nullable<DoubleType> > NullableDoubleArray;
typedef Array<UInt32Type> IntArray;
typedef Array< Nullable<UInt32Type> > NullableIntArray;

// ALTERNATE SYSTEM WITHOUT NULL TEMPLATES
// see arrays_alt.h and builder_alt.h
typedef ArrayAlt<UInt32Type> IntArrayAlt;
typedef ArrayAlt<DoubleType> DoubleArrayAlt;

double randDouble()
{
    return (double)rand() / RAND_MAX;
}

bool randBool()
{
    return (rand() % 2);
}

uint32_t randInt() 
{
  return rand() % ARRAY_LEN;
}

template <typename T, typename U>
void templateTest(T* doubArray, 
                      U* intArray)
{
  templateTestResult.total.start();
  auto *result = Take(doubArray, intArray);
  templateTestResult.total.finish();

  //std::cout << result->to_string() << std::endl;
}

void altTest(DoubleArrayAlt* doubleArrayAlt, 
                 IntArrayAlt* intArrayAlt)
{
  altTestResult.total.start();
  auto *result = TakeAlt(doubleArrayAlt, intArrayAlt);
  altTestResult.total.finish();

  //std::cout << result->to_string() << std::endl;
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
    bitmask2[i] = randBool();
    nullCount2 += is_null;
    nullFreeBitmask2[i] = false;
  }

  // arrays
  DoubleArray *doubArray = new DoubleArray(doubleValues, ARRAY_LEN);
  NullableDoubleArray *nullableDoubArray = new NullableDoubleArray(doubArray, bitmask1, nullCount1);
  IntArray *intArray = new IntArray(intValues, INT_ARRAY_LEN);
  NullableIntArray *nullableIntArray = new NullableIntArray(intArray, bitmask2, nullCount2);

  DoubleArrayAlt *doubleArrayAlt = new DoubleArrayAlt(doubleValues, nullFreeBitmask1, ARRAY_LEN);
  DoubleArrayAlt *doubleArrayAltWithNulls = new DoubleArrayAlt(doubleValues, bitmask1, ARRAY_LEN, nullCount1);
  IntArrayAlt *intArrayAlt = new IntArrayAlt(intValues, nullFreeBitmask2, INT_ARRAY_LEN);
  IntArrayAlt *intArrayAltWithNulls = new IntArrayAlt(intValues, bitmask2, INT_ARRAY_LEN, nullCount2);

  // see take.h for take implementation

  for (int32_t i = 0; i < REPS; i++) {
    templateTest(doubArray, intArray);
    templateTest(nullableDoubArray, intArray);
    templateTest(doubArray, nullableIntArray);
    templateTest(nullableDoubArray, nullableIntArray);

    altTest(doubleArrayAlt, intArrayAlt);
    altTest(doubleArrayAltWithNulls, intArrayAlt);
    altTest(doubleArrayAlt, intArrayAltWithNulls);
    altTest(doubleArrayAltWithNulls, intArrayAltWithNulls);
  }

  std::cout << "Take with alternate implementation:" << std::endl;
  std::cout << altTestResult.to_string() << std::endl;

  std::cout << "Take using Nullable<T> template:" << std::endl;
  std::cout << templateTestResult.to_string() << std::endl;

  delete[] doubleValues;
  delete[] bitmask1;
  delete[] nullFreeBitmask1;
  delete[] intValues;
  delete[] bitmask2;
  delete[] nullFreeBitmask2;
}