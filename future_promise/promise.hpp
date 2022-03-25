#pragma once

#include "future.hpp"

#include <memory>
#include <twist/stdlike/atomic.hpp>
#include <iostream>
#include <variant>

// #include "channel.hpp"

namespace stdlike {

template <typename T>
class Promise {
 public:
  Promise() : channel_(std::make_shared<stdlike::detail::Channel<T>>()) {
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;

  // One-shot
  Future<T> MakeFuture() {
    return std::move(Future<T>(channel_));
  }

  // One-shot
  // Fulfill promise with value
  void SetValue(T value) {
    channel_->Setval(std::move(value));
  }

  // One-shot
  // Fulfill promise with exception
  void SetException(std::exception_ptr ex) {
    channel_->Setexc(std::forward<std::exception_ptr>(ex));
  }

 private:
  std::shared_ptr<stdlike::detail::Channel<T>> channel_;
};

}  // namespace stdlike
