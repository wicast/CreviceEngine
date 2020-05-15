#include "common/ResourceManager.h"

RID ResourceManager::createShaderPack(const std::string &vertPath,const std::string &fragPath) {
    auto vertShaderCode = readFile(vertPath);
    auto fragShaderCode = readFile(fragPath);

    VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

    myvk::ShaderPack shaderpack;
    shaderpack.fragShaderModule = fragShaderModule;
    shaderpack.vertShaderModule = vertShaderModule;
    return addShaderPack(shaderpack);
}

VkShaderModule ResourceManager::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(vkDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

void ResourceManager::destoryShaderPack(RID rid) {
    myvk::ShaderPack sp = getShaderPack(rid);

    vkDestroyShaderModule(vkDevice, sp.fragShaderModule, nullptr);
    vkDestroyShaderModule(vkDevice, sp.vertShaderModule, nullptr);
}