namespace arrow {

struct DataType {};

template<typename T>
struct PrimitiveType : public DataType {
  public:
    typedef T value_type;
};

template<typename T>
class Nullable : public DataType {
  public:
    typedef typename T::value_type value_type;
};

typedef PrimitiveType<double> DoubleType;
typedef PrimitiveType<uint32_t> UInt32Type;

typedef Nullable<DoubleType> NullableDoubleType;
typedef Nullable<UInt32Type> NullableUInt32Type;

}
