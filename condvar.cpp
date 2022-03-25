#pragma once

#include <twist/stdlike/atomic.hpp>

#include <cstdint>

namespace stdlike {

class CondVar {
 public:
  // Mutex - BasicLockable
  // https://en.cppreference.com/w/cpp/named_req/BasicLockable
  template <class Mutex>
  void Wait(Mutex& mutex) {
    uint32_t cur_ticket = ticket_.load();
    mutex.unlock();
    ticket_.FutexWait(cur_ticket);
    mutex.lock();
  }
  void NotifyOne() {
    ticket_.fetch_add(1);
    ticket_.FutexWakeOne();
  }

  void NotifyAll() {
    ticket_.fetch_add(1);
    ticket_.FutexWakeAll();
  }

 private:
  twist::stdlike::atomic<uint32_t> ticket_{0};
};

}  // namespace stdlike

