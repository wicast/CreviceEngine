#pragma once

#include "3rd/volk/volk_imp.h"
#include "common/ResourceManager.h"
#include "containers/Container.h"
#include "containers/glfw/glfwContainerImpl.h"
#include "render/Context.h"
#include "render/GpuResourceManager.h"
#include "render/ShaderPack.h"
#include "render/resource/Model.h"
#include "render/resource/Texture.h"
#include "render/vulkan/windowContext.h"

namespace crevice {
class RenderServer {
 public:
  // TODO use container api
  GLFWContainer* container;

  VkContext* vkContext;
  WindowContext* windowContext;
  GpuResourceManager* gpuRManager;
  // TODO there should be multiple rendergraph
  //   RenderGraph* mRendergraph;

  bool enableValidationLayers = true;

  RenderServer(){

  };

  RenderServer(GLFWContainer* container)
      : container(container){

        };

  void init() {
    createVkContext();
    createGPUResourceManager();
    createWindowContext();
    createCommandPool();
  };

  void setContainer(GLFWContainer* container) { this->container = container; };

  void createVkContext() {
    vkContext = new VkContext();

    vkContext->enableValidationLayers = enableValidationLayers;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    volkInitialize();
    vkContext->createInstance(vkUtil::getRequiredExtensions(
        vkContext->enableValidationLayers, glfwExtensions, glfwExtensionCount));
    volkLoadInstance(vkContext->instance);

    container->createSurface(vkContext->instance, &(vkContext->surface));
    vkContext->pickPhysicalDevice();
    vkContext->createLogicalDevice();

    vkContext->setupDebugMessenger();
  }
  void createGPUResourceManager() {
    gpuRManager = new GpuResourceManager();

    gpuRManager->initManager(vkContext);
    gpuRManager->initDescriptorPool();
  };

  void createWindowContext() {
    windowContext = new WindowContext{};
    gpuRManager->createSwapChain(*container, *windowContext);
    gpuRManager->createSwapChainImageViews(*windowContext, 1);

    //TODO no need for this
    vkContext->windowContext = windowContext;
  };

  void createCommandPool() { vkContext->createCommandPool(); }

  void createSwapChainDepthResources() {
    VkFormat depthFormat = findDepthFormat();
    gpuRManager->createImage(
        windowContext->swapChainExtent.width,
        windowContext->swapChainExtent.height, 1, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, windowContext->depthImage,
        windowContext->depthImageMemory);
    windowContext->depthImageView = gpuRManager->createImageView(
        windowContext->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    gpuRManager->transitionImageLayout(
        windowContext->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
  }

  void waitIdle() { vkDeviceWaitIdle(vkContext->device); }

  // cleanup functions

  void destory() {}

  // Util functions

  int32_t swapChainSize() { return GpuResourceManager::swapChainSize; }

  VkFormat swapChainFormat() { return windowContext->swapChainImageFormat; }

  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);

  VkFormat findDepthFormat();
};

}  // namespace crevice
