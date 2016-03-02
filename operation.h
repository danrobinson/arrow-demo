template<typename OperationType>
class Operation {
public:
  explicit Operation(OperationType& next) : next_(next) {}

  // passes null results through
  void skip() {
    next_.skip();
  }

protected:
  OperationType next_;
};