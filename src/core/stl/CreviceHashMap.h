#pragma once

#include "EASTL/hash_map.h"

namespace crevice
{
    template <typename Key, typename T, typename Hash = eastl::hash<Key>, typename Predicate = eastl::equal_to<Key>, 
			  typename Allocator = EASTLAllocatorType, bool bCacheHashCode = false>
    using HashMap = eastl::hash_map<Key, T, Hash , Predicate,  Allocator, bCacheHashCode>;
} // namespace crevice
