#pragma once

#include "3rd/volk/volk_imp.h"
#include "stl/CreviceVector.h"

class WindowContext {
 private:
  /* data */

 public:
  VkSwapchainKHR swapChain;
  crevice::Vector<VkImage> swapChainImages;
  VkPresentModeKHR swapChainPresentMode;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  crevice::Vector<VkImageView> swapChainImageViews;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  float lastX, lastY;
  bool firstMouse = true;
};
