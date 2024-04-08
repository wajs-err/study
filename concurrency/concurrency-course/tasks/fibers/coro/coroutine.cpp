#include "coroutine.hpp"

#include <twist/ed/local/ptr.hpp>

#include <wheels/core/assert.hpp>
#include <wheels/core/compiler.hpp>

Coroutine::Coroutine(Routine /*routine*/) {
  // Not implemented
}

void Coroutine::Resume() {
  // Not implemented
}

void Coroutine::Suspend() {
  // Not implemented
}

bool Coroutine::IsCompleted() const {
  return true;  // Not implemented
}
