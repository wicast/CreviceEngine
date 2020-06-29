#include <vector>

#include <volk.h>
#include "render/ShaderPack.h"


class Material {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    
    myvk::ShaderPack shaders;
};
