#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdint>

namespace stdlike {

class Mutex {
   public:
    void Lock() {
        uint32_t a = 0;
        if (!locked_.compare_exchange_strong(a, 1)) {
            do {
                if (a == 1) {
                    locked_.compare_exchange_strong(a, 2);
                }
                twist::ed::futex::Wait(locked_, 2);
                a = 0;
                locked_.compare_exchange_strong(a, 2);
            } while (a != 0);
        }
    }

    void Unlock() {
        if (locked_.fetch_sub(1) != 1) {
            auto wake_key = twist::ed::futex::PrepareWake(locked_);
            locked_.store(0);
            twist::ed::futex::WakeOne(wake_key);
        }
    }

    // BasicLockable
    // https://en.cppreference.com/w/cpp/named_req/BasicLockable

    void lock() {  // NOLINT
        Lock();
    }

    void unlock() {  // NOLINT
        Unlock();
    }

   private:
    twist::ed::stdlike::atomic<uint32_t> locked_{0};
};

}  // namespace stdlike
