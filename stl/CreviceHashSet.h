#pragma once

#include "EASTL/hash_set.h"

namespace crevice
{
    template <typename Value, typename Hash = eastl::hash<Value>, typename Predicate = eastl::equal_to<Value>, 
			  typename Allocator = EASTLAllocatorType, bool bCacheHashCode = false>
    using HashSet = eastl::hash_set<Value, Hash, Predicate, Allocator , bCacheHashCode>;
} // namespace crevice
