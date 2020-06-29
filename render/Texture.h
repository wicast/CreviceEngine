#ifndef MYVK_MYTEXTURE_H
#define MYVK_MYTEXTURE_H
#include <volk.h>

namespace myvk {
    struct MyTexture {
        unsigned char * pixels;
        uint32_t width;
        uint32_t hight;
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        VkSampler textureSampler;
    };
}

#endif