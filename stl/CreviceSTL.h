#ifndef CREVICE_STL
#define CREVICE_STL 1

#include "stl/CreviceHashMap.h"
#include "stl/CreviceHashSet.h"
#include "stl/CreviceSharedPtr.h"
#include "stl/CreviceString.h"
#include "stl/CreviceVector.h"
#include "stl/CreviceVectorMap.h"
#include "stl/CreviceVectorSet.h"
#include "stl/CreviceDeque.h"
#include "stl/CreviceQueue.h"

namespace crevice {
//iter    
template <typename Container>
EA_CPP14_CONSTEXPR inline auto end(Container& container) {
  return eastl::end(container);
}

template <typename Container>
EA_CPP14_CONSTEXPR inline auto begin(Container& container) {
  return eastl::begin(container);
}

//alg
template <typename BidirectionalIterator>
	inline void reverse(BidirectionalIterator first, BidirectionalIterator last) {
        eastl::reverse(first,last);
    }

}  // namespace crevice

#endif