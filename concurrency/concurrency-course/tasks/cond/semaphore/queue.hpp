#pragma once

#include "tagged_semaphore.hpp"

#include <deque>

// Bounded Blocking Multi-Producer/Multi-Consumer (MPMC) Queue

template <typename T>
class BoundedBlockingQueue {
   public:
    explicit BoundedBlockingQueue(size_t capacity)
        : buffer_(),
          input_semaphore_(capacity),
          output_semaphore_(0),
          mutex_semaphore_(1) {
    }

    void Put(T value) {
        auto token = input_semaphore_.Acquire();
        auto lock = mutex_semaphore_.Acquire();
        buffer_.push_back(std::move(value));
        mutex_semaphore_.Release(std::move(lock));
        output_semaphore_.Release(std::move(token));
    }

    T Take() {
        auto token = output_semaphore_.Acquire();
        auto lock = mutex_semaphore_.Acquire();
        auto ret = std::move(buffer_.front());
        buffer_.pop_front();
        mutex_semaphore_.Release(std::move(lock));
        input_semaphore_.Release(std::move(token));
        return ret;
    }

   private:
    // Tags
    struct CommonTag {};
    struct MutexTag {};

   private:
    std::deque<T> buffer_;
    TaggedSemaphore<CommonTag> input_semaphore_;
    TaggedSemaphore<CommonTag> output_semaphore_;
    TaggedSemaphore<MutexTag> mutex_semaphore_;
};
