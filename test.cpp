#include <iostream>
#include <sstream>
#include "take.h"
#include "take_alt.h" // alternate implementation of take


using namespace arrow;

#define ARRAY_LEN 10000
#define INT_ARRAY_LEN 10000
#define REPS 10

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

// random boolean, 20% of which are true, 80% are false
bool randBool()
{
    return (rand() % 5 == 0);
}

uint32_t randInt() 
{
  return rand() % ARRAY_LEN;
}

template <typename T, typename U>
uint32_t templateTest(T* doubArray, U* intArray)
{
  TimePoint begin = std::chrono::steady_clock::now();
  auto *result = Take(doubArray, intArray);
  TimePoint end = std::chrono::steady_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
}

uint32_t altTest(DoubleArrayAlt* doubleArrayAlt, IntArrayAlt* intArrayAlt)
{
  TimePoint begin = std::chrono::steady_clock::now();
  auto *result = TakeAlt(doubleArrayAlt, intArrayAlt);
  TimePoint end = std::chrono::steady_clock::now();

  return std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
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
  DoubleArray *doubArray = new DoubleArray(doubleValues, ARRAY_LEN);
  NullableDoubleArray *nullableDoubArray = new NullableDoubleArray(doubArray, bitmask1, nullCount1);
  IntArray *intArray = new IntArray(intValues, INT_ARRAY_LEN);
  NullableIntArray *nullableIntArray = new NullableIntArray(intArray, bitmask2, nullCount2);

  DoubleArrayAlt *doubleArrayAlt = new DoubleArrayAlt(doubleValues, nullFreeBitmask1, ARRAY_LEN);
  DoubleArrayAlt *doubleArrayAltWithNulls = new DoubleArrayAlt(doubleValues, bitmask1, ARRAY_LEN, nullCount1);
  IntArrayAlt *intArrayAlt = new IntArrayAlt(intValues, nullFreeBitmask2, INT_ARRAY_LEN);
  IntArrayAlt *intArrayAltWithNulls = new IntArrayAlt(intValues, bitmask2, INT_ARRAY_LEN, nullCount2);

  // see take.h for take implementation


  struct results {
    public:
      uint32_t no_nulls;
      uint32_t nullable_doubles;
      uint32_t nullable_integers;
      uint32_t both_nullable;

      std::string to_string() {
        std::stringstream ss;
        ss << "No Nulls: " << no_nulls << std::endl;
        ss << "Nullable Doubles: " << nullable_doubles << std::endl;
        ss << "Nullable Integers: " << nullable_integers << std::endl;
        ss << "Both Nullable: " << both_nullable << std::endl;
        return ss.str();
      }
  } altTestResult, templateTestResult;

  for (int32_t i = 0; i < REPS; i++) {
    templateTestResult.no_nulls += templateTest(doubArray, intArray);
    templateTestResult.nullable_doubles += templateTest(nullableDoubArray, intArray);
    templateTestResult.nullable_integers += templateTest(doubArray, nullableIntArray);
    templateTestResult.both_nullable += templateTest(nullableDoubArray, nullableIntArray);

    altTestResult.no_nulls += altTest(doubleArrayAlt, intArrayAlt);
    altTestResult.nullable_doubles += altTest(doubleArrayAltWithNulls, intArrayAlt);
    altTestResult.nullable_integers += altTest(doubleArrayAlt, intArrayAltWithNulls);
    altTestResult.both_nullable += altTest(doubleArrayAltWithNulls, intArrayAltWithNulls);
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