#ifndef TYPES_H_
#define TYPES_H_

namespace arrow {

struct DataType {};

template<typename T>
struct PrimitiveType : public DataType {
  public:
    typedef T c_type;
};

template<typename T>
class Nullable : public DataType {
  public:
    typedef typename T::c_type c_type;
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

template<typename T>
using nullify = typename std::conditional<is_nullable<T>::value, T, Nullable<T> >::type;

template<typename T, typename U>
using propagate_null = typename std::conditional<is_nullable<T>::value, nullify<U>, U>::type;

} // namespace arrow

#endif
