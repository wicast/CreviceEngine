/**
 * @file glfwContainerImpl.h
 * @author wicast (wicast@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include <functional>

#include "../Container.h"
#include "glfw.h"

namespace crevice {

/**
 * @brief GLFW specific window container
 *
 */
class GLFWContainer : public Container {
 private:
  GLFWwindow* window;

  void createGLFWWindow(int width, int height, const char* title,
                        GLFWmonitor* monitor = nullptr,
                        GLFWwindow* share = nullptr);

 public:
  std::function<void(GLFWwindow*)> inputProcessor;
  static GLFWframebuffersizefun framebufferResizeCallback;
  static GLFWcursorposfun mouse_callback_dispatch;

  /**
   * @brief Create a Window object
   *
   * @param width
   * @param height
   * @param title
   */
  void createWindow(int width, int height, const char* title);
  /**
   * @brief Create a Surface object
   *
   * @param instance
   * @param surface
   * @param allocator
   */
  void createSurface(VkInstance instance, VkSurfaceKHR* surface,
                     const VkAllocationCallbacks* allocator = nullptr);

  void getFramebufferSize(int* width, int* height);

  /**
   * @brief Set the Up Input object
   *
   * @param pointer
   */
  void setUpInput(void* pointer);

  /**
   * @brief Process Input And Event
   *
   */
  void processInputAndEvent() {
    inputProcessor(window);
    glfwPollEvents();
  }

  /**
   * @brief Set close state
   *
   */
  void setClose() { glfwSetWindowShouldClose(window, true); }

  /**
   * @brief set Window should close
   *
   * @return int
   */
  int windowShouldClose() { return glfwWindowShouldClose(window); };

  const char ** getVkRequiredInstanceExtensions(uint32_t* count) {
    return glfwGetRequiredInstanceExtensions(count);
  }

  /**
   * @brief Construct a new GLFWContainer object
   *
   */
  GLFWContainer();

  /**
   * @brief Construct a new GLFWContainer object
   *
   * @param width
   * @param height
   * @param title
   * @param monitor
   * @param share
   */
  GLFWContainer(int width, int height, const char* title) {
    createWindow(width, height, title);
  };
  /**
   * @brief Destroy the GLFWContainer object
   *
   */
  ~GLFWContainer();
};
}  // namespace crevice
