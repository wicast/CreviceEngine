/**
 * @file HelloTriangleApplication.h
 * @author wicast (wicast@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

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

#include "3rd/GLFW/glfw.h"
#include "3rd/volk/volk_imp.h"
#include "common/CVTimer.h"
#include "common/GLMath.h"
#include "common/ResourceManager.h"
#include "common/AssetResourceBridge.hpp"
#include "assets_manager/AssetManager.hpp"
#include "components/Camera.h"
#include "containers/glfw/glfwContainerImpl.h"
#include "flecs.h"
#include "graphic/vk_render/Context.h"
#include "graphic/vk_render/GpuResourceManager.h"
#include "graphic/vk_render/RenderServer.h"
#include "graphic/vk_render/Uniform.h"
#include "graphic/vk_render/VkResourceBridge.hpp"
#include "graphic/vk_render/renderGraph/RenderGraph.h"
#include "graphic/vk_render/vulkan/windowContext.h"
#include "stl/CreviceSharedPtr.h"

class HelloTriangleApplication {
 private:
  crevice::GLFWContainer container;
  crevice::RenderServer* renderServer;
  AssetManager* assetManager;
  ResourceManager* rManager;

  AssetResourceBridge* assetResourceBridge;
  crevice::VkResourceBridge* vkResourceBridge;

  flecs::world ecs;

  // TODO
  bool framebufferResized = false;

  void serverSetup();

  void setupECS();

  void mainLoop();

  void processInput(GLFWwindow* window);

  void cleanupSwapChain();

  void recreateSwapChain();

  void cleanup();

  static void framebufferResizeCallback(GLFWwindow* window,
                                        int width,
                                        int height) {
    auto app = reinterpret_cast<HelloTriangleApplication*>(
        glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
  }

 public:
  static HelloTriangleApplication* event_handling_instance;

  static void mouse_callback_dispatch(GLFWwindow* window,
                                      double xpos,
                                      double ypos) {
    if (event_handling_instance)
      event_handling_instance->mouse_callback(window, xpos, ypos);
  }

  void setEventHandling() { event_handling_instance = this; }

  void mouse_callback(GLFWwindow* window, double xpos, double ypos);

  void run();
};
