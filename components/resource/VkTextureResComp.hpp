#pragma once

#include "common/Resource.h"
#include "stl/CreviceSTL.h"
#include <future>
#include "render/resource/VkTexture.hpp"

struct VkTextureResComp
{
    eastl::vector<RID> unloaded;
    eastl::vector_map<RID, std::shared_future<crevice::SharedPtr<crevice::VkTexture>>> textureLoading;
    eastl::vector_map<RID, crevice::SharedPtr<crevice::VkTexture>> textureLoaded;
};
