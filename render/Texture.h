#ifndef MYVK_MYTEXTURE_H
#define MYVK_MYTEXTURE_H
#include <volk.h>

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

#endif