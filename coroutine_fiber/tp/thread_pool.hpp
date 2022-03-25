#pragma once

#include "exe/tp/blocking_queue.hpp"
#include <exe/tp/task.hpp>
#include <vector>

#include <twist/stdlike/thread.hpp>

namespace exe::tp {

// Fixed-size pool of worker threads

class ThreadPool {
 public:
  explicit ThreadPool(size_t workers);
  ~ThreadPool();

  // Non-copyable
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  // Schedules task for execution in one of the worker threads
  // Do not use directly, use tp::Submit instead
  void Submit(Task task);

  // Waits until outstanding work count has reached zero
  void WaitIdle();

  // Stops the worker threads as soon as possible
  // Pending tasks will be discarded
  void Stop();
  void WorkersBody();

  // Locates current thread pool from worker thread
  static ThreadPool* Current();

 private:
  UnboundedBlockingQueue<tp::Task> queue_;
  std::vector<twist::stdlike::thread> threads_;
  twist::strand::stdlike::atomic<bool> closed_{false};
  twist::strand::stdlike::atomic<uint32_t> curretly_active_{0};
  // Worker threads, task queue, etc
};

inline ThreadPool* Current() {
  return ThreadPool::Current();
}

}  // namespace exe::tp
