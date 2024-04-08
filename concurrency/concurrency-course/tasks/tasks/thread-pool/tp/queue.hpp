#pragma once

#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <optional>

namespace tp {

// Unbounded blocking multi-producers/multi-consumers (MPMC) queue

template <typename T>
class UnboundedBlockingQueue {
 public:
  bool Put(T) {
    return false;  // Not implemented
  }

  std::optional<T> Take() {
    return std::nullopt;  // Not implemented
  }

  void Close() {
    // Not implemented
  }

 private:
  // Buffer
};

}  // namespace tp
