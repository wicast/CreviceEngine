#pragma once

#include <functional>

#include "3rd/GLFW/glfw.h"
#include "containers/Container.h"

namespace crevice {
class GLFWContainer : public Container {
 private:
 public:
  GLFWwindow* window;
  std::function<void(GLFWwindow*)> inputProcessor;
  static GLFWframebuffersizefun framebufferResizeCallback;
  static GLFWcursorposfun mouse_callback_dispatch;

  void createWindow(int width,
                    int height,
                    const char* title,
                    GLFWmonitor* monitor = nullptr,
                    GLFWwindow* share = nullptr);
  void createSurface(VkInstance instance,
                     VkSurfaceKHR* surface,
                     const VkAllocationCallbacks* allocator = nullptr);

  void setUpInput(void* pointer);

  void processInputAndEvent() {
    inputProcessor(window);
    glfwPollEvents();
  }

  void setClose() { glfwSetWindowShouldClose(window, true); }

  int windowShouldClose() { return glfwWindowShouldClose(window); };

  GLFWContainer();
  GLFWContainer(int width,
                int height,
                const char* title,
                GLFWmonitor* monitor = nullptr,
                GLFWwindow* share = nullptr) {
    createWindow(width, height, title, monitor, share);
  };
  ~GLFWContainer();
};
}  // namespace crevice
