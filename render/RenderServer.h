#ifndef MYVK_RENDERSERVER_H
#define MYVK_RENDERSERVER_H 1

#include "render/Model.h"
#include "render/Texture.h"
#include "render/ShaderPack.h"
#include "common/ResourceManager.h"
#include <vulkan/vulkan.h>

class RenderServer {
    VkDevice device;
    VkInstance instance;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkPresentModeKHR swapChainPresentMode;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;

    ResourceManager resourceManager;

    public:
    void bindResourceManager(ResourceManager resm) {
        resourceManager = resm;
    }



    RID createTextureFromPic(std::string picPath);
    RID createShader(std::string vertPath, std::string fragPath);
    RID removeShader(RID rid);

    

    private:
    VkShaderModule createShaderModule(const std::vector<char> &code);
};

#endif