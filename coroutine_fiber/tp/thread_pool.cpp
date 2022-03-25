#include <exe/tp/thread_pool.hpp>

#include <twist/util/thread_local.hpp>

namespace exe::tp {

////////////////////////////////////////////////////////////////////////////////

static twist::util::ThreadLocalPtr<ThreadPool> pool;

////////////////////////////////////////////////////////////////////////////////

void ThreadPool::WorkersBody() {
  while (!closed_.load()) {
    std::optional<tp::Task> current_task = queue_.Take();
    if (current_task.has_value()) {
      try {
        current_task.value()();
      } catch (...) {
      }
      if (curretly_active_.fetch_sub(1) == 1) {
        curretly_active_.notify_one();
      }
    }
  }
}

ThreadPool::ThreadPool(size_t workers) {
  auto run = [this] {
    pool = this;
    WorkersBody();
  };

  for (std::size_t i = 0; i < workers; i++) {
    threads_.push_back(twist::stdlike::thread(run));
  }
}

ThreadPool::~ThreadPool() {
  assert(closed_);
}

void ThreadPool::Submit(tp::Task task) {
  curretly_active_.fetch_add(1);
  queue_.Put(std::move(task));
}

void ThreadPool::WaitIdle() {
  size_t curr_act_v;
  while ((curr_act_v = curretly_active_.load()) > 0) {
    curretly_active_.wait(curr_act_v);
  }
}

void ThreadPool::Stop() {
  if (!closed_.load()) {
    closed_.store(true);
    queue_.Close();
    for (auto&& empl : threads_) {
      empl.join();
    }
  }
}

ThreadPool* ThreadPool::Current() {
  return pool;
}
}  // namespace exe::tp
