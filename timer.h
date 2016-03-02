#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>


namespace arrow {

typedef std::chrono::steady_clock::time_point TimePoint;

class Test {
public:

  void start() {
    begin_ = std::chrono::steady_clock::now();
  }

  void finish() {
    end_ = std::chrono::steady_clock::now();
    add();
  }

  void add() {
    sum_ += difference();
  }

  uint32_t difference() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end_ - begin_).count();
  }

  uint32_t sum() {
    return sum_;
  }

private:
  TimePoint begin_;
  TimePoint end_;
  uint32_t sum_ = 0;
};

class Timer {
  public:
    void reset() {
      current = * new Test();
    }

    std::string to_string() {
      std::stringstream ss;
      ss << "Total: " << current.sum() << std::endl;      
      return ss.str();
    }
    Test current;
  } timer;

} // namespace arrow

#endif