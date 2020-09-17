#pragma once

#include "EASTL/queue.h"

namespace crevice
{
    template <typename T, typename Container = eastl::deque<T, EASTLAllocatorType, DEQUE_DEFAULT_SUBARRAY_SIZE(T)> >
    using Queue = eastl::queue<T,Container>;

    
} // namespace crevice