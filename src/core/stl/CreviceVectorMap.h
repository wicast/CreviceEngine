#pragma once

#include "EASTL/vector_map.h"

namespace crevice {
template <typename Key, typename T, typename Compare = eastl::less<Key>,
          typename Allocator = EASTLAllocatorType,
          typename RandomAccessContainer =
              eastl::vector<eastl::pair<Key, T>, Allocator> >
using VectorMap =
    eastl::vector_map<Key, T, Compare, Allocator, RandomAccessContainer>;
}  // namespace crevice
