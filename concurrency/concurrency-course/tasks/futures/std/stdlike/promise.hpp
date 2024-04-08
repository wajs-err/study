#pragma once

#include <stdlike/future.hpp>

#include <memory>

namespace stdlike {

template <typename T>
class Promise {
 public:
  Promise() {
    // Not implemented
  }

  // Non-copyable
  Promise(const Promise&) = delete;
  Promise& operator=(const Promise&) = delete;

  // Movable
  Promise(Promise&&) = default;
  Promise& operator=(Promise&&) = default;

  // One-shot
  Future<T> MakeFuture() {
    throw std::runtime_error("Not Implemented");  // Not implemented
  }

  // One-shot
  // Fulfill promise with value
  void SetValue(T) {
    // Not implemented
  }

  // One-shot
  // Fulfill promise with exception
  void SetException(std::exception_ptr) {
    // Not implemented
  }

 private:
  // ???
};

}  // namespace stdlike
