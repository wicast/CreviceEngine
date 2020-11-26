/**
 * @file Texture.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once
#include "3rd/volk/volk_imp.h"

namespace crevice {
    /**
     * @brief crevice texture
     * 
     */
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
