#pragma once

#include "volk_imp.h"

namespace crevice
{
    struct VkTexture {
        uint32_t width;
        uint32_t height;
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        //TODO sampler can be shared
        VkSampler textureSampler;
    };
    
} // namespace crevice
