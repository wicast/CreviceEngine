#ifndef CREVICE_STL_VECTOR
#define CREVICE_STL_VECTOR 1

#include "EASTL/vector.h"

namespace crevice
{
    template <typename T, typename Allocator = EASTLAllocatorType>
    using Vector = eastl::vector<T,Allocator>;
} // namespace crevice

#endif