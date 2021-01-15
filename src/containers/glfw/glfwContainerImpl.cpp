#include "glfwContainerImpl.h"

namespace crevice {
GLFWContainer::GLFWContainer() {}

GLFWContainer::~GLFWContainer() {}

void GLFWContainer::createGLFWWindow(int width, int height, const char* title,
                                     GLFWmonitor* monitor, GLFWwindow* share) {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, title, monitor, share);
}

void GLFWContainer::createWindow(int width, int height, const char* title) {
  createGLFWWindow(width, height, title);
}

void GLFWContainer::createSurface(VkInstance instance, VkSurfaceKHR* surface,
                                  const VkAllocationCallbacks* allocator) {
  if (glfwCreateWindowSurface(instance, window, allocator, surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

void GLFWContainer::getFramebufferSize(int* width, int* height) {
  glfwGetFramebufferSize(window, width, height);
}

void GLFWContainer::setUpInput(void* userPointer) {
  glfwSetWindowUserPointer(window, userPointer);
  glfwSetFramebufferSizeCallback(window,
                                 GLFWContainer::framebufferResizeCallback);
  // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, GLFWContainer::mouse_callback_dispatch);
}

GLFWframebuffersizefun GLFWContainer::framebufferResizeCallback = nullptr;
GLFWcursorposfun GLFWContainer::mouse_callback_dispatch = nullptr;

}  // namespace crevice
