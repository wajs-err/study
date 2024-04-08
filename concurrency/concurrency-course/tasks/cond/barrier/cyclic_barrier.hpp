#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>
#include <twist/ed/stdlike/condition_variable.hpp>

#include <cstdlib>

class CyclicBarrier {
   public:
    explicit CyclicBarrier(size_t participants)
        : participants_(participants),
          current_waiters_(0),
          can_leave_(),
          can_enter_(),
          mutex_(),
          left_waiters_(0) {
    }

    void ArriveAndWait() {
        std::unique_lock lock(mutex_);
        while (left_waiters_ != 0) {
            can_enter_.wait(lock);
        }
        ++current_waiters_;
        while (current_waiters_ != participants_) {
            can_leave_.wait(lock);
        }
        if (current_waiters_ == participants_) {
            can_leave_.notify_all();
        }
        ++left_waiters_;
        if (left_waiters_ == participants_) {
            left_waiters_ = 0;
            current_waiters_ = 0;
            can_enter_.notify_all();
        }
    }

   private:
    size_t participants_;
    size_t current_waiters_;
    twist::ed::stdlike::condition_variable can_leave_;
    twist::ed::stdlike::condition_variable can_enter_;
    twist::ed::stdlike::mutex mutex_;
    size_t left_waiters_;
};
