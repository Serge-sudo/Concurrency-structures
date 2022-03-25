#pragma once

#include "tagged_semaphore.hpp"

#include <deque>

namespace solutions {

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BlockingQueue {
 public:
  explicit BlockingQueue(size_t capacity)
      : semaphore_put_(capacity), semaphore_pop_(0), semaphore_for_move_(1) {
  }

  // Inserts the specified element into this queue,
  // waiting if necessary for space to become available.
  void Put(T value) {
    auto token = semaphore_put_.Acquire();

    auto ticket = semaphore_for_move_.Acquire();
    deque_.push_back(std::move(value));
    semaphore_for_move_.Release(std::move(ticket));

    semaphore_pop_.Release(std::move(token));
  }

  // Retrieves and removes the head of this queue,
  // waiting if necessary until an element becomes available
  T Take() {
    auto token = semaphore_pop_.Acquire();

    auto ticket = semaphore_for_move_.Acquire();
    T item = std::move(deque_.front());
    deque_.pop_front();
    semaphore_for_move_.Release(std::move(ticket));

    semaphore_put_.Release(std::move(token));
    return item;
  }

 private:
  std::deque<T> deque_;
  TaggedSemaphore<T> semaphore_put_;
  TaggedSemaphore<T> semaphore_pop_;
  TaggedSemaphore<T> semaphore_for_move_;
};

}  // namespace solutions

