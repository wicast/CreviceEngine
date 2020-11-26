/**
 * @file ShaderPack.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "3rd/volk/volk_imp.h"

namespace crevice {
    struct ShaderPack {
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}
