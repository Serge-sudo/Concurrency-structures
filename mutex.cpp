#pragma once

#include <twist/stdlike/atomic.hpp>
#include <cstdlib>

namespace stdlike {

class Mutex {
 public:
  void Lock() {
    waiting_cnt_.fetch_add(1);
    while (locked_.exchange(1) != 0u) {
      locked_.FutexWait(1);
    }
    waiting_cnt_.fetch_sub(1);
  }

  void Unlock() {
    locked_.store(0);
    if (waiting_cnt_.load() >= 1) {
      locked_.FutexWakeOne();
    }
  }

 private:
  twist::stdlike::atomic<uint32_t> locked_{0};
  twist::stdlike::atomic<uint32_t> waiting_cnt_{0};
};

}  // namespace stdlike
