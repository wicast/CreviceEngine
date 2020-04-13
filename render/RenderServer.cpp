#include <render/RenderServer.h>

RID RenderServer::createTextureFromPic(std::string picPath){
    return 0;
}

RID RenderServer::createShader(std::string vertPath, std::string fragPath){
    auto vertShaderCode = ResourceManager::readFile(vertPath);
    auto fragShaderCode = ResourceManager::readFile(fragPath);
    myvk::ShaderPack shaderPack;
    shaderPack.vertShaderModule = createShaderModule(vertShaderCode);
    shaderPack.fragShaderModule = createShaderModule(fragShaderCode);

    return resourceManager.addShaderPack(shaderPack);
}

RID RenderServer::removeShader(RID rid) {
    myvk::ShaderPack pack = resourceManager.shaders.find(rid)->second;
    vkDestroyShaderModule(device, pack.fragShaderModule, nullptr);
    vkDestroyShaderModule(device, pack.vertShaderModule, nullptr);
    resourceManager.shaders.erase(rid);
    return rid;
}

VkShaderModule RenderServer::createShaderModule(const std::vector<char> &code) {
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}