#pragma once
#include "3rd/volk/volk_imp.h"

namespace crevice {
    struct CVTexture {
        uint32_t width;
        uint32_t hight;
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        //TODO sampler can be shared
        VkSampler textureSampler;
    };
}
