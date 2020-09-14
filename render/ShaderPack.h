#ifndef MYVK_SHADERPACK_H
#define MYVK_SHADERPACK_H 1

#include <volk.h>

namespace crevice {
    struct ShaderPack {
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}

#endif