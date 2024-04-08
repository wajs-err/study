#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/sys.hpp>

#include <cstdlib>

namespace stdlike {

class Mutex {
   public:
    void Lock() {
        if (locked_.exchange(1) != 0) {
            uint32_t a = 0;
            do {
                twist::ed::futex::Wait(locked_, a + 1);
                a = locked_.fetch_add(1);
            } while (a != 0);
        }
    }

    void Unlock() {
        auto wake_key = twist::ed::futex::PrepareWake(locked_);
        locked_.store(0);
        twist::ed::futex::WakeOne(wake_key);
    }

   private:
    twist::ed::stdlike::atomic<uint32_t> locked_{0};
};

}  // namespace stdlike
