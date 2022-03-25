#pragma once

#include <tp/blocking_queue.hpp>
#include <tp/task.hpp>
#include <twist/stdlike/atomic.hpp>
#include <twist/stdlike/thread.hpp>
#include <vector>

namespace tp {

// Fixed-size pool of worker threads

class ThreadPool {
 public:
  explicit ThreadPool(size_t workers);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Schedules task for execution in one of the worker threads
  void Submit(Task task);

  // Waits until outstanding work count has reached zero
  void WaitIdle();

  void WorkersBody();

  // Stops the worker threads as soon as possible
  // Pending tasks will be discarded
  void Stop();

  // Locates current thread pool from worker thread
  static ThreadPool* Current();

 private:
  UnboundedBlockingQueue<tp::Task> queue_;
  std::vector<twist::stdlike::thread> threads_;
  twist::stdlike::atomic<bool> closed_{false};
  twist::stdlike::atomic<uint32_t> curretly_active_{0};
};

inline ThreadPool* Current() {
  return ThreadPool::Current();
}

}  // namespace tp
