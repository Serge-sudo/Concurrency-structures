#include <exe/coroutine/impl.hpp>
#include <iostream>
#include <wheels/support/assert.hpp>
#include <wheels/support/compiler.hpp>

namespace exe::coroutine {

CoroutineImpl::CoroutineImpl(Routine routine, wheels::MutableMemView stack)
    : routine_(std::move(routine)) {
  mycontex_.Setup(stack, this);
}

void CoroutineImpl::Run() {
  try {
    routine_();
  } catch (...) {
    except_ = std::current_exception();
  }
  finished_ = true;
  Suspend();
  std::abort();
}

void CoroutineImpl::Resume() {
  if (finished_) {
    return;
  }
  currentcontex_.SwitchTo(mycontex_);
  if (except_) {
    std::rethrow_exception(except_);
  }
}

void CoroutineImpl::Suspend() {
  mycontex_.SwitchTo(currentcontex_);
}

bool CoroutineImpl::IsCompleted() const {
  return finished_;
}

}  // namespace exe::coroutine
