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
    Test total;
    Test in_function;
    Test in_loop;
    Test adding;

    std::string to_string() {
      std::stringstream ss;
      ss << "Total: " << total.sum() << std::endl;      
      // ss << "In Function: " << in_function.sum() << std::endl;      
      // ss << "In Loop: " << in_loop.sum() << std::endl;      
      // ss << "Adding: " << adding.sum() << std::endl;      
      //ss << "No Nulls: " << no_nulls << std::endl;
      //ss << "Nullable Doubles: " << nullable_doubles << std::endl;
      //ss << "Nullable Integers: " << nullable_integers << std::endl;
      //ss << "Both Nullable: " << both_nullable << std::endl;
      return ss.str();
    }

  } altTestResult, templateTestResult;

} // namespace arrow

#endif