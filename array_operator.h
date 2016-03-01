template<typename OperationType>
class Operation {
  explicit Operation(OperationType* next) : _next(next) {}

  // passes null results through
  operator() () {
    next();
  }
  
protected:
  OperationType *_next;
}