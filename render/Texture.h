#ifndef MYVK_MYTEXTURE_H
#define MYVK_MYTEXTURE_H
#include <vulkan/vulkan.h>

namespace myvk {
    struct MyTexture {
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
    };
}

#endif