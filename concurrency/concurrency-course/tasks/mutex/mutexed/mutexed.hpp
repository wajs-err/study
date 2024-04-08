#pragma once

#include <mutex>
#include <twist/ed/stdlike/mutex.hpp>

//////////////////////////////////////////////////////////////////////

/*
 * Safe API for mutual exclusion
 *
 * Usage:
 *
 * Mutexed<std::vector<Apple>> apples;
 *
 * {
 *   auto owner_ref = apples->Acquire();
 *   owner_ref->push_back(Apple{});
 * }  // <- release ownership
 *
 */

template <typename T, class Mutex = twist::ed::stdlike::mutex>
class Mutexed {
    class OwnerRef {
       public:
        OwnerRef(T& object, Mutex& mutex)
            : object_(object),
              guard_(mutex) {
        }

        ~OwnerRef() {
        }

        T* operator->() {
            return &object_;
        }

       private:
        T& object_;
        std::lock_guard<Mutex> guard_;
    };

   public:
    // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
    template <typename... Args>
    explicit Mutexed(Args&&... args)
        : object_(std::forward<Args>(args)...) {
    }

    OwnerRef Acquire() {
        return OwnerRef(object_, mutex_);
    }

   private:
    T object_;
    Mutex mutex_;  // Guards access to object_
};

//////////////////////////////////////////////////////////////////////

template <typename T>
auto Acquire(Mutexed<T>& object) {
    return object.Acquire();
}
