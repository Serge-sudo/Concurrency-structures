#pragma once

#include <exe/fibers/core/api.hpp>
#include <exe/coroutine/impl.hpp>
#include <context/stack.hpp>

namespace exe::fibers {

// Fiber = Stackful coroutine + Scheduler (Thread pool)

class Fiber {
 public:
  // ~ System calls
  void Schedule();
  void Yield();

  static Fiber& Self();
  static Scheduler& GetScheduler();
  Fiber(Scheduler& scheduler, Routine routine);
  Fiber(const Fiber& parent, Routine routine);
  ~Fiber();

 private:
  // Task
  void Step();

 private:
  Scheduler* scheduler_;
  context::Stack stack_;
  coroutine::CoroutineImpl my_coroutine_;
};

}  // namespace exe::fibers
