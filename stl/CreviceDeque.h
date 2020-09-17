#pragma once

#include "EASTL/deque.h"

namespace crevice
{

    template <typename T, typename Allocator = EASTLAllocatorType, unsigned kDequeSubarraySize = DEQUE_DEFAULT_SUBARRAY_SIZE(T)>
    using Deque = eastl::deque<T, Allocator, kDequeSubarraySize>;

    
} // namespace crevice