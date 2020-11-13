//
// Created by wicas on 2020/1/5.
//
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
#include "components/Camera.h"
#include "containers/glfw/glfwContainerImpl.h"
#include "flecs.h"
#include "render/Context.h"
#include "render/GpuResourceManager.h"
#include "render/RenderServer.h"
#include "render/Uniform.h"
#include "render/renderGraph/RenderGraph.h"
#include "render/vulkan/windowContext.h"
#include "stl/CreviceSharedPtr.h"

class HelloTriangleApplication {
 private:
  crevice::GLFWContainer container;
  crevice::RenderServer* renderServer;

  flecs::world ecs;

  // TODO
  // ResourceManager resourceManager;

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
