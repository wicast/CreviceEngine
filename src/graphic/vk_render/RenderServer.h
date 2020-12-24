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

#include "Container.h"
#include "Context.h"
#include "GpuResourceManager.h"
#include "ShaderPack.h"
#include "common/ResourceManager.h"
#include "glfw/glfwContainerImpl.h"
#include "renderGraph/RenderGraph.h"
#include "resource/Image.h"
#include "resource/Model.h"
#include "volk_imp.h"
#include "vulkan/windowContext.h"

namespace crevice {
class RenderServer {
 public:
  VkContext* vkContext;
  VkWindowContext* defaultWindowContext;
  GpuResourceManager* gpuRManager;

  static RenderServer* getInstance() {
    if (instance == nullptr) {
      instance = new RenderServer();
    }
    return instance;
  }

  void setContainer(GLFWContainer* container) { this->container = container; };

  void init() {
    createObjs();

    createVkContext();
    pickRenderDevice();
    createGPUResourceManager();
    createWindowContext();
    createCommandPool();

    createSwapChainDepthResources();
  };

  void waitIdle() { vkDeviceWaitIdle(vkContext->device); }

  // cleanup functions

  void destory() {}

  // Util functions

  uint8_t swapChainSize() { return defaultWindowContext->swapChainSize; }

  VkFormat swapChainFormat() {
    return defaultWindowContext->swapChainImageFormat;
  }
  
  VkFormat findDepthFormat();

 private:
  static RenderServer* instance;

  RenderServer(){};

  static size_t currentFrame;
  // TODO use container api
  GLFWContainer* container;

  // TODO there should be multiple rendergraph
  //   RenderGraph* mRendergraph;

  bool enableValidationLayers = true;

  void createObjs() {
    vkContext = new VkContext();
    gpuRManager = new GpuResourceManager();
    gpuRManager->initManager(vkContext);
    defaultWindowContext = new VkWindowContext{};
  }

  void createVkContext() {
    vkContext->enableValidationLayers = enableValidationLayers;

    uint32_t glfwExtensionCount = 0;
    // TODO required extension should be portable
    auto glfwExtensions =
        container->getVkRequiredInstanceExtensions(&glfwExtensionCount);

    volkInitialize();
    vkContext->createInstance(vkUtil::getRequiredExtensions(
        vkContext->enableValidationLayers, glfwExtensions, glfwExtensionCount));
    volkLoadInstance(vkContext->instance);
  }

  void pickRenderDevice() {
    container->createSurface(vkContext->instance,
                             &defaultWindowContext->surface);
    vkContext->pickPhysicalDevice(defaultWindowContext->surface);
    vkContext->createLogicalDeviceAndQueue();

    vkContext->setupDebugMessenger();
  }

  void createGPUResourceManager() { gpuRManager->initDescriptorPool(); };

  void createWindowContext() {
    vkContext->createSwapChain(*container, *defaultWindowContext);
    gpuRManager->createSwapChainImageViews(*defaultWindowContext, 1);
  };

  void createCommandPool() { vkContext->createCommandPool(); }

  void createSwapChainDepthResources() {
    VkFormat depthFormat = findDepthFormat();
    gpuRManager->createImage(
        defaultWindowContext->swapChainExtent.width,
        defaultWindowContext->swapChainExtent.height, 1, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, defaultWindowContext->depthImage,
        defaultWindowContext->depthImageMemory);
    defaultWindowContext->depthImageView =
        gpuRManager->createImageView(defaultWindowContext->depthImage,
                                     depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    gpuRManager->transitionImageLayout(
        defaultWindowContext->depthImage, depthFormat,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
  }

  VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
};

}  // namespace crevice
