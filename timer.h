#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>


namespace arrow {

using TimePoint = std::chrono::steady_clock::time_point;

class Timer {
  public:
    void reset() {
      total_ = 0;
    }

    void start() {
      begin_ = std::chrono::steady_clock::now();
    }

    void finish() {
      end_ = std::chrono::steady_clock::now();
      total_ += difference();
    }

    uint32_t difference() {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(end_ - begin_).count();
    }

    std::string to_string() {
      std::stringstream ss;
      ss << "Total: " << difference() << std::endl;      
      return ss.str();
    }
  private:
    TimePoint begin_;
    TimePoint end_;
    int32_t total_;
  } timer;

} // namespace arrow

#endif