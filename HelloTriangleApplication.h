//
// Created by wicas on 2020/1/5.
//
#pragma once

#include "3rd/volk/volk_imp.h"

#include "3rd/GLFW/glfw.h"

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
#include "render/RenderServer.h"
#include "containers/glfw/glfwContainerImpl.h"
#include "components/Camera.h"

class HelloTriangleApplication {
 private:
crevice::GLFWContainer container;
  crevice::RenderServer renderServer;

  std::chrono::high_resolution_clock::time_point lastTime =
      std::chrono::high_resolution_clock::now();
  ;
  float frameDeltaTime;

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

  ResourceManager resourceManager;

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

  void serverSetup();

  void createResourceManager();

  void createRenderGraph();
  void setPassAndCompileRenderGraph();
  void createRenderAble();

  void switchCommandBuffer(RID *commandId);


  void createPerPassUniformBuffers();

  void createObjectTextureImage();

  void createDepthResources();

  void loadObj1Model();

  void initVulkan();

  void updateUniformBuffer(uint32_t currentImage);

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

  static void mouse_callback_dispatch(GLFWwindow *window, double xpos,
                                      double ypos) {
    if (event_handling_instance)
      event_handling_instance->mouse_callback(window, xpos, ypos);
  }

  void setEventHandling() { event_handling_instance = this; }

  void mouse_callback(GLFWwindow *window, double xpos, double ypos);

  void run();
};

