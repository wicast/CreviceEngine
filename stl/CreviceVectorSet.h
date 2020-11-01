#pragma once

#include "EASTL/vector_set.h"

namespace crevice
{
    template <typename Key, typename Compare = eastl::less<Key>, typename Allocator = EASTLAllocatorType, 
			  typename RandomAccessContainer = eastl::vector<Key, Allocator> >
    using VectorSet = eastl::vector_set<Key, Compare, Allocator,RandomAccessContainer>;
} // namespace crevice
