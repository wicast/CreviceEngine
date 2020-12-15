#pragma once

#include "common/Resource.h"
#include "stl/CreviceSTL.h"
#include <future>
#include "resource/Image.h"

struct ImageResComp
{
    eastl::vector<RID> unloaded;
    eastl::vector_map<RID, std::shared_future<crevice::SharedPtr<crevice::Image2D>>> imagesLoading;
    eastl::vector_map<RID, crevice::SharedPtr<crevice::Image2D>> imagesLoaded;
    // HACK this will replace with a broadcast event system
    eastl::vector<RID> imagesLoadedContinue;

};
