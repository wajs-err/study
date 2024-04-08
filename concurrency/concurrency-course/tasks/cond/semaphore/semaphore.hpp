#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>

class Semaphore {
   public:
    explicit Semaphore(size_t tokens)
        : tokens_(tokens),
          mutex_(),
          not_empty_() {
    }

    void Acquire() {
        std::unique_lock lock(mutex_);
        while (tokens_ == 0) {
            not_empty_.wait(lock);
        }
        --tokens_;
    }

    void Release() {
        std::lock_guard guard(mutex_);
        ++tokens_;
        not_empty_.notify_one();
    }

   private:
    size_t tokens_;
    twist::ed::stdlike::mutex mutex_;
    twist::ed::stdlike::condition_variable not_empty_;
};
