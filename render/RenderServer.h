#ifndef MYVK_RENDERSERVER_H
#define MYVK_RENDERSERVER_H 1

#include "render/resource/Model.h"
#include "render/resource/Texture.h"
#include "render/ShaderPack.h"
#include "common/ResourceManager.h"
#include <volk.h>

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

};

#endif