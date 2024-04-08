#pragma once

#include <memory>
#include <cassert>

namespace stdlike {

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  // One-shot
  // Wait for result (value or exception)
  T Get() {
    throw std::runtime_error("Not implemented");
  }

 private:
  Future(/*???*/) {
  }

 private:
  // ???
};

}  // namespace stdlike
