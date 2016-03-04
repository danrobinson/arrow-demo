#include <iostream>
#include <sstream>
#include "take.h"
#include "timer.h"
#include "take_alt.h" // alternate implementation of take

using namespace arrow;

#define ARRAY_LEN 10000
#define INT_ARRAY_LEN 1000000
#define REPS 10

typedef std::chrono::steady_clock::time_point TimePoint;

// see types.h for type definitions
typedef PrimitiveType<double> DoubleType;
typedef PrimitiveType<uint32_t> UInt32Type;

// see arrays_alt.h and builder_alt.h
typedef ArrayAlt<UInt32Type> IntArrayAlt;
typedef ArrayAlt<DoubleType> DoubleArrayAlt;

double randDouble()
{
    return rand() * (double)rand() / RAND_MAX;
}

// random boolean, 20% true, 80% false
bool randBool()
{
    return rand() % 5 == 0;
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
  const Array<DoubleType> doubArray(doubleValues, ARRAY_LEN);
  const Array<Nullable<DoubleType> >  nullableDoubArray(doubArray, bitmask1, nullCount1);
  const Array<UInt32Type> intArray(intValues, INT_ARRAY_LEN);
  const Array<Nullable<UInt32Type> > nullableIntArray(intArray, bitmask2, nullCount2);

  // without a nullable template
  const ArrayAlt<DoubleType> doubleArrayAlt(doubleValues, nullFreeBitmask1, ARRAY_LEN);
  const ArrayAlt<DoubleType> doubleArrayAltWithNulls(doubleValues, bitmask1, ARRAY_LEN, nullCount1);
  const ArrayAlt<UInt32Type> intArrayAlt(intValues, nullFreeBitmask2, INT_ARRAY_LEN);
  const ArrayAlt<UInt32Type> intArrayAltWithNulls (intValues, bitmask2, INT_ARRAY_LEN, nullCount2);

  timer.reset();
  timer.start();
  for (int32_t i = 0; i < REPS; i++) {
    Take(intArray, doubArray);
    Take(intArray, nullableDoubArray);
    Take(nullableIntArray, doubArray);
    Take(nullableIntArray, nullableDoubArray);
  }
  timer.finish();
  std::cout << "Template method: " << REPS << " reps with " << INT_ARRAY_LEN << " indices and " << ARRAY_LEN << " elements" << std::endl;
  std::cout << timer.to_string() << std::endl;

  timer.reset();
  timer.start();
  for (int32_t i = 0; i < REPS; i++) {
    TakeAlt(doubleArrayAlt, intArrayAlt);
    TakeAlt(doubleArrayAltWithNulls, intArrayAlt);
    TakeAlt(doubleArrayAlt, intArrayAltWithNulls);
    TakeAlt(doubleArrayAltWithNulls, intArrayAltWithNulls);
  }
  timer.finish();
  std::cout << "Alternate method: " << REPS << " reps with " << INT_ARRAY_LEN << " indices and " << ARRAY_LEN << " elements" << std::endl;
  std::cout << timer.to_string() << std::endl;

  delete[] doubleValues;
  delete[] bitmask1;
  delete[] nullFreeBitmask1;
  delete[] intValues;
  delete[] bitmask2;
  delete[] nullFreeBitmask2;
}