#ifndef CREVICE_STL_SHARED_PTR
#define CREVICE_STL_SHARED_PTR 1

#include "EASTL/shared_ptr.h"

namespace crevice
{
    template <class T>
    using SharedPtr = eastl::shared_ptr<T>;

    

    template <typename T, typename... Args>
	SharedPtr<T> make_shared(Args&&... args) {
        return eastl::make_shared<T>(eastl::forward<Args>(args)...);
    }
} // namespace crevice

#endif