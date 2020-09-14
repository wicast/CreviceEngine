#ifndef CREVICE_STL_SHARED_PTR
#define CREVICE_STL_SHARED_PTR 1

#include "EASTL/shared_ptr.h"

namespace crevice
{
    template <class T>
    using SharedPtr = eastl::shared_ptr<T>;
} // namespace crevice

#endif