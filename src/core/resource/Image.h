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

#include "eastl/vector.h"

namespace crevice {
    
    struct IMageRawData {
        unsigned char* rawData;
        size_t size;
    };

    /**
     * @brief crevice texture
     * 
     */
    struct Image2D {
        uint32_t width;
        uint32_t height;

        /**
         * @brief raw data which is Mip level 0
         * 
         */
        IMageRawData rawL0;
        uint32_t mipLevels = 1;
        eastl::vector<IMageRawData> mipData;
    };
}
