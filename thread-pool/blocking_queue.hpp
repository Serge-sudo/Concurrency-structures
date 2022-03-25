#pragma once

#include <twist/stdlike/mutex.hpp>
#include <twist/stdlike/condition_variable.hpp>

#include <optional>
#include <deque>
// #include <utility>

namespace tp {

// Unbounded blocking multi-producers/multi-consumers queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  bool Put(T value) {
    std::unique_lock<twist::stdlike::mutex> guard(mutex_);
    if (open_) {
      queue_.push_back(std::move(value));
      cv_pop_.notify_one();
      return true;
    }
    return false;
  }

  std::optional<T> Take() {
    std::unique_lock<twist::stdlike::mutex> guard(mutex_);
    if (!open_ && queue_.size() == 0) {
      return std::nullopt;
    }
    while (queue_.size() == 0) {
      cv_pop_.wait(guard);
      if (queue_.size() == 0 && !open_) {
        return std::nullopt;
      }
    }
    T obj = std::move(queue_.front());
    queue_.pop_front();
    return obj;
  }

  void Close() {
    CloseImpl(false);
  }

  void Cancel() {
    CloseImpl(true);
  }

 private:
  void CloseImpl(bool clear) {
    std::unique_lock<twist::stdlike::mutex> guard(mutex_);
    open_ = false;
    if (clear) {
      queue_.clear();
    }
    cv_pop_.notify_all();
  }

 private:
  twist::stdlike::condition_variable cv_pop_;
  twist::stdlike::mutex mutex_;
  bool open_ = true;
  std::deque<T> queue_;
};

}  // namespace tp
