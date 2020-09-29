#ifndef MYVK_WINDOWCONTEXT_H
#define MYVK_WINDOWCONTEXT_H 1

#include <GLFW/glfw3.h>
#include <volk.h>
#include "stl/CreviceVector.h"

class WindowContext {
 private:
  /* data */

 public:
  GLFWwindow* window;
  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkPresentModeKHR swapChainPresentMode;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  crevice::Vector<VkImageView> swapChainImageViews;
  float lastX, lastY;
  bool firstMouse = true;
};

#endif