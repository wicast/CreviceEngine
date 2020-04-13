#include <vector>

#include <vulkan/vulkan.h>
#include "render/ShaderPack.h"


class Material {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    
    myvk::ShaderPack shaders;
};
