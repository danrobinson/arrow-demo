#ifndef TYPES_H_
#define TYPES_H_

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

// traits
template<typename T>
class is_nullable {
public:
  static const bool value = false;
};

template<typename T>
class is_nullable<Nullable<T> > {
public:
  static const bool value = true;
};

} // namespace arrow

#endif
