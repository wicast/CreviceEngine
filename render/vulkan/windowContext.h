/**
 * @file windowContext.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "3rd/volk/volk_imp.h"
#include "stl/CreviceVector.h"

/**
 * @brief Window Context
 * 
 */
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
