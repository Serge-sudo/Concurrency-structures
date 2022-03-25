#pragma once

#include <twist/stdlike/mutex.hpp>
#include <twist/stdlike/condition_variable.hpp>

// std::lock_guard, std::unique_lock
#include <mutex>
#include <cstdint>

namespace solutions {

// CyclicBarrier allows a set of threads to all wait for each other
// to reach a common barrier point

// The barrier is called cyclic because
// it can be re-used after the waiting threads are released.

class CyclicBarrier {
 public:
  explicit CyclicBarrier(size_t participants) {
    participants_cnt_ = participants;
    waiting_for_ = participants;
  }

  // Blocks until all participants have invoked Arrive()
  void Arrive() {
    std::unique_lock<twist::stdlike::mutex> guard(mutex_);
    size_t my_cycle = current_cycle_;
    --waiting_for_;
    if (waiting_for_ > 0) {
      while (current_cycle_ == my_cycle) {
        cv_.wait(guard);
      }
    } else {
      current_cycle_ += 1;
      cv_.notify_all();
      waiting_for_ = participants_cnt_;
    }
  }

 private:
  twist::stdlike::mutex mutex_;
  twist::stdlike::condition_variable cv_;
  size_t participants_cnt_, waiting_for_;
  size_t current_cycle_ = 0;
};

}  // namespace solutions
