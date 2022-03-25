#include <exe/fibers/core/fiber.hpp>
#include <exe/fibers/core/stacks.hpp>

#include <twist/util/thread_local.hpp>

namespace exe::fibers {

//////////////////////////////////////////////////////////////////////
static twist::util::ThreadLocalPtr<Fiber> current;

Fiber::Fiber(Scheduler& scheduler, Routine routine)
    : scheduler_(&scheduler),
      stack_(AllocateStack()),
      my_coroutine_(std::move(routine), stack_.View()) {
}

void Fiber::Schedule() {
  scheduler_->Submit([this] {
    current = this;
    my_coroutine_.Resume();
    Step();
  });
}

Fiber::~Fiber() {
  ReleaseStack(std::move(stack_));
}

void Fiber::Yield() {
  my_coroutine_.Suspend();
}

void Fiber::Step() {
  if (my_coroutine_.IsCompleted()) {
    delete this;
  } else {
    Schedule();
  }
}

Fiber& Fiber::Self() {
  return *current;
}

Scheduler& Fiber::GetScheduler() {
  return *Fiber::Self().scheduler_;
}

//////////////////////////////////////////////////////////////////////

// API Implementation

void Go(Scheduler& scheduler, Routine routine) {
  (new Fiber(scheduler, std::move(routine)))->Schedule();
}

void Go(Routine routine) {
  (new Fiber(Fiber::GetScheduler(), std::move(routine)))->Schedule();
}

namespace self {

void Yield() {
  Fiber::Self().Yield();
}

}  // namespace self

}  // namespace exe::fibers
