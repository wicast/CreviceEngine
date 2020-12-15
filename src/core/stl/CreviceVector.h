#pragma once

#include "EASTL/vector.h"

namespace crevice
{
    template <typename T, typename Allocator = EASTLAllocatorType>
    using Vector = eastl::vector<T,Allocator>;
} // namespace crevice
