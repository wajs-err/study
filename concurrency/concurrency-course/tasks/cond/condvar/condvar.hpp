#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdint>

namespace stdlike {

class CondVar {
   public:
    // Mutex - BasicLockable
    // https://en.cppreference.com/w/cpp/named_req/BasicLockable
    template <class Mutex>
    void Wait(Mutex& mutex) {
        ++waiting_;
        mutex.unlock();

        while (notified_.load() == 0) {
            twist::ed::futex::Wait(notified_, 0);
        }
        notified_.fetch_sub(1);

        mutex.lock();
        --waiting_;
    }

    void NotifyOne() {
        auto wake_key = twist::ed::futex::PrepareWake(notified_);
        notified_.fetch_add(1);
        twist::ed::futex::WakeOne(wake_key);
    }

    void NotifyAll() {
        auto wake_key = twist::ed::futex::PrepareWake(notified_);
        notified_.store(waiting_);
        twist::ed::futex::WakeAll(wake_key);
    }

   private:
    uint32_t waiting_{0};
    twist::ed::stdlike::atomic<uint32_t> notified_{0};
};

}  // namespace stdlike
