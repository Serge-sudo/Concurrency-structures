#include <exe/fibers/core/stacks.hpp>

using context::Stack;

namespace exe::fibers {

//////////////////////////////////////////////////////////////////////

class StackAllocator {
 public:
  Stack Allocate() {
    mutex_.lock();
    if (!stacks_.empty()) {
      Stack free_stack = std::move(stacks_.back());
      stacks_.pop_back();
      mutex_.unlock();
      return free_stack;
    }
    mutex_.unlock();
    return AllocateNewStack();
  }

  void Release(Stack stack) {
    std::lock_guard guard(mutex_);
    stacks_.push_back(std::move(stack));
  }

 private:
  static Stack AllocateNewStack() {
    static const size_t kStackPages = 16;  // 16 * 4KB = 64KB
    return Stack::AllocatePages(kStackPages);
  }

 private:
  twist::stdlike::mutex mutex_;
  std::vector<Stack> stacks_;
  // Pool
};

//////////////////////////////////////////////////////////////////////

StackAllocator allocator;

context::Stack AllocateStack() {
  return allocator.Allocate();
}

void ReleaseStack(context::Stack stack) {
  allocator.Release(std::move(stack));
}

}  // namespace exe::fibers
