#pragma once

#include "3rd/volk/volk_imp.h"

namespace crevice {
    struct ShaderPack {
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}
