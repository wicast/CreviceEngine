/**
 * @file RenderServer.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-26
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "volk_imp.h"
#include "common/ResourceManager.h"
#include "Container.h"
#include "glfw/glfwContainerImpl.h"
#include "Context.h"
#include "GpuResourceManager.h"
#include "ShaderPack.h"
#include "resource/Model.h"
#include "resource/Image.h"
#include "vulkan/windowContext.h"

#include "renderGraph/RenderGraph.h"

namespace crevice {
class RenderServer {
 private:
  static RenderServer* instance;

  RenderServer(){};

 public:
  static size_t currentFrame;
  // TODO use container api
  GLFWContainer* container;

  VkContext* vkContext;
  WindowContext* windowContext;
  GpuResourceManager* gpuRManager;
  // TODO there should be multiple rendergraph
  //   RenderGraph* mRendergraph;

  bool enableValidationLayers = true;

  static RenderServer* getInstance() {
    if (instance == nullptr) {
      instance = new RenderServer();
    }
    return instance;
  }

  void init() {
    createVkContext();
    createGPUResourceManager();
    createWindowContext();
    createCommandPool();

    createSwapChainDepthResources();
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

    // TODO no need for this
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