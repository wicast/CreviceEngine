//
// Created by wicas on 2020/1/5.
//
#ifndef MYVKLEARN_HELLOTRIANGLEAPPLICATION_H
#define MYVKLEARN_HELLOTRIANGLEAPPLICATION_H

#include <vulkan/vulkan.h>

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

#include "common/GLMath.h"
#include "common/ResourceManager.h"
#include "render/Context.h"
#include "render/vulkan/windowContext.h"
#include "render/GpuResourceManager.h"
#include "scene/Camera.h"

struct MyImage {
  uint32_t mipLevels;
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;
};

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
  float moveCount = 0;

  VkContext vkContext;

  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkRenderPass renderPass;
  VkPipeline graphicsPipeline;

  uint32_t mipLevels;
  VkImage obj1TextureImage;
  VkDeviceMemory obj1TextureImageMemory;
  VkImageView obj1TextureImageView;
  VkSampler obj1TextureSampler;

  MyImage specImage;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  ResourceManager resourceManager;
  GpuResourceManager gpuResourceManager;

  std::vector<Vertex> obj1Vertices;
  std::vector<uint32_t> obj1Indices;

  RID obj1;

  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;

  std::vector<VkBuffer> uniformBuffers;
  std::vector<VkDeviceMemory> uniformBuffersMemory;

  std::vector<VkFramebuffer> swapChainFramebuffers;
  VkCommandPool commandPool;
  VkDescriptorPool descriptorPool;
  std::vector<VkDescriptorSet> descriptorSets;
  std::vector<VkCommandBuffer> commandBuffers;

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

  void createFramebuffers();

  void createCommandPool();

  void createCommandBuffers();

  void createSyncObjects();

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);

  VkFormat findDepthFormat();

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer &buffer,
                    VkDeviceMemory &bufferMemory);

  bool hasStencilComponent(VkFormat format);

  void createVertexBuffer();

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  VkCommandBuffer beginSingleTimeCommands();

  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

  void createIndexBuffer();

  void createDescriptorSetLayout();

  void createUniformBuffers();

  void createDescriptorPool();

  void createDescriptorSets();

  void createObjectTextureImage();

  void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels);

  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout,
                             uint32_t transitionImageLayout);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

  void createTextureImageView();

  void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                   VkFormat format, VkImageTiling tiling,
                   VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                   VkImage &image, VkDeviceMemory &imageMemory);

  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);

  void createTextureSampler();

  void createDepthResources();

  // void loadModelFor(std::vector<Vertex>& vertices, std::vector<uint32_t>&
  // indices, std::string modelPath);

  void loadObj1Model();

  void initVulkan();

  void updateUniformBuffer(uint32_t currentImage);

  void drawFrame();

  void mainLoop();

  void processInput(GLFWwindow *window);

  void cleanupSwapChain();

  void recreateSwapChain();

  void cleanup();

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height) {
    auto app = reinterpret_cast<HelloTriangleApplication *>(
        glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

  bool checkValidationLayerSupport();

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

  void keycallback(GLFWwindow *window, int key, int scancode, int action,
                   int mods);

  void mouse_callback(GLFWwindow *window, double xpos, double ypos);

  void run();
};

#endif  // MYVKLEARN_HELLOTRIANGLEAPPLICATION_H
