//
// Created by wicas on 2020/1/5.
//
#ifndef MYVKLEARN_HELLOTRIANGLEAPPLICATION_H
#define MYVKLEARN_HELLOTRIANGLEAPPLICATION_H

// #define VK_USE_PLATFORM_WIN32_KHR
#include <volk.h>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "stl/CreviceSharedPtr.h"

#include "common/GLMath.h"
#include "common/ResourceManager.h"
#include "render/Context.h"
#include "render/GpuResourceManager.h"
#include "render/Uniform.h"
#include "render/vulkan/windowContext.h"
#include "render/renderGraph/RenderGraph.h"
#include "components/Camera.h"

class HelloTriangleApplication {
 private:
  // GLFWwindow *window;
  // float lastX, lastY;
  WindowContext windowContext;

  // bool firstMouse = true;
  std::chrono::high_resolution_clock::time_point lastTime =
      std::chrono::high_resolution_clock::now();
  ;
  float frameDeltaTime;

  VkContext vkContext;

  crevice::RenderGraph mRendergraph;
  uint32_t mainPassId;
  uint32_t swapId;
  uint32_t depId;

  crevice::SharedPtr<VkDescriptorSetLayout> descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkRenderPass renderPass;
  VkPipeline graphicsPipeline;

  RID obj1TexId;
  RID specTexId;

  // TODO via manager
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  ResourceManager resourceManager;
  GpuResourceManager gpuResourceManager;

  RID obj1;
  RID obj2;

  crevice::RenderAble objr1;
  crevice::RenderAble objr2;

  crevice::Vector<crevice::PerPassRenderAble> perpassList;
  crevice::Vector<crevice::RenderAble> renderList;

  crevice::Vector<VkBuffer> cameraUniformBuffers;
  crevice::Vector<VkDeviceMemory> cameraUniformBuffersMemory;

  std::vector<VkFramebuffer> swapChainFramebuffers;

  RID descriptorSets;

  RID commandBuffers;
  RID commandBuffers2;
  RID *drawingBuffersId;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  size_t currentFrame = 0;
  std::vector<VkFence> imagesInFlight;
  std::vector<VkFence> inFlightFences;

  bool framebufferResized = false;

  // Scene setups
  glm::vec3 camPosition = glm::vec3(1.0f, 0.0f, 0.0f);
  glm::vec3 camDirect = glm::vec3(-1.f, 0.0f, 0.0f);
  Camera camera;

  void createResourceManager();

  void createVkContext();

  void createGpuResourceManager();

  void createSurface();

  VkShaderModule createShaderModule(const std::vector<char> &code);

  void createGraphicsPipeline();

  void createRenderPass();
  void createRenderGraph();
  void setPassAndCompileRenderGraph();
  void createRenderAble();

  void createFramebuffers();

  void createCommandBuffers();

  void switchCommandBuffer(RID *commandId);

  void createSyncObjects();

  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);

  VkFormat findDepthFormat();

  void createDescriptorSetLayout();

  void createPerPassUniformBuffers();

  void initDescriptorPool();

  void createDescriptorSets();

  void createObjectTextureImage();

  void createTextureSampler();

  void createDepthResources();

  void loadObj1Model();

  void initVulkan();

  void updateUniformBuffer(uint32_t currentImage);

  void drawFrame();
  void drawFrameWithFrameGraph();

  void mainLoop();

  void processInput(GLFWwindow *window);

  void cleanupSwapChain();

  void recreateSwapChain();

  void cleanup();

  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height) {
    auto app = reinterpret_cast<HelloTriangleApplication *>(
        glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

 public:
  static HelloTriangleApplication *event_handling_instance;

  static void keycallback_dispatch(GLFWwindow *window, int key, int scancode,
                                   int action, int mods) {
    if (event_handling_instance)
      event_handling_instance->keycallback(window, key, scancode, action, mods);
  }

  static void mouse_callback_dispatch(GLFWwindow *window, double xpos,
                                      double ypos) {
    if (event_handling_instance)
      event_handling_instance->mouse_callback(window, xpos, ypos);
  }

  void setEventHandling() { event_handling_instance = this; }

  // TODO keycallback is passive callback with different deltatime
  void keycallback(GLFWwindow *window, int key, int scancode, int action,
                   int mods);

  void mouse_callback(GLFWwindow *window, double xpos, double ypos);

  void run();
};

#endif  // MYVKLEARN_HELLOTRIANGLEAPPLICATION_H
