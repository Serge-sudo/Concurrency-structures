#include <memory>
#include <cassert>
#include <twist/stdlike/atomic.hpp>
#include <variant>
#include <iostream>

namespace stdlike::detail {
template <typename T>
class Channel {
 public:
  Channel()
      : flag_(twist::stdlike::atomic<uint32_t>(0)),
        val_(std::variant<std::exception_ptr, T>()) {
  }

  T Get() {
    while (flag_.load() == 0) {
      flag_.FutexWait(0);
    }

    if(auto val = std::get_if<1>(&val_)){
      return std::move(std::get<1>(val_));
    }
    std::rethrow_exception(std::get<0>(val_));
  }

  void Setexc(std::exception_ptr ex) {
    std::get<0>(val_) = std::move(ex);
    flag_.store(1);
    flag_.FutexWakeOne();
  }

  void Setval(T value) {
    // val_.template emplace<1>(std::forward<T>(value));
    val_ = std::forward<T>(value);
    flag_.store(1);
    flag_.FutexWakeOne();
  }

 private:
  twist::stdlike::atomic<uint32_t> flag_;
  std::variant<std::exception_ptr, T> val_;
};

}  // namespace stdlike::detail
