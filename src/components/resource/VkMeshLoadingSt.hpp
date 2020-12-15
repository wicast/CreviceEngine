#pragma once

#include "common/Resource.h"
#include <future>
#include "graphic/vk_render/resource/VkMesh.hpp"
#include "stl/CreviceSharedPtr.h"

/**
 * @brief Vulkan mash loading status
 * 
 */
struct VkMeshLoadingSt
{
    RID rid;
    bool waitMem = false;
    bool loading = false;
    std::shared_future<crevice::SharedPtr<VkMesh>> vkMeshF;
};
