#pragma once

#include <context/stack.hpp>
#include <twist/stdlike/mutex.hpp>
#include <vector>

namespace exe::fibers {

context::Stack AllocateStack();
void ReleaseStack(context::Stack stack);

}  // namespace exe::fibers
