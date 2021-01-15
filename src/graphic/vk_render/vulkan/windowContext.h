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

#include "../Context.h"
#include "containers/Container.h"
#include "stl/CreviceVector.h"
#include "volk_imp.h"

/**
 * @brief Window Context
 *
 */
class VkWindowContext {
 private:
  /* data */

 public:
  crevice::Container* container;
  VkContext* context;

  VkSurfaceKHR surface;
  VkSwapchainKHR swapChain;
  uint8_t swapChainSize;
  crevice::Vector<VkImage> swapChainImages;
  VkPresentModeKHR swapChainPresentMode;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  crevice::Vector<VkImageView*> swapChainImageViews;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  float lastX, lastY;
  bool firstMouse = true;

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
      return capabilities.currentExtent;
    } else {
      int width, height;
      container->getFramebufferSize(&width, &height);

      VkExtent2D actualExtent = {static_cast<uint32_t>(width),
                                 static_cast<uint32_t>(height)};

      actualExtent.width = std::max(
          capabilities.minImageExtent.width,
          std::min(capabilities.maxImageExtent.width, actualExtent.width));
      actualExtent.height = std::max(
          capabilities.minImageExtent.height,
          std::min(capabilities.maxImageExtent.height, actualExtent.height));

      return actualExtent;
    }
  }

  void createSwapChain() {
    SwapChainSupportDetails swapChainSupport =
        vkUtil::querySwapChainSupport(context->physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat =
        vkUtil::chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode =
        vkUtil::chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    // TODO select swapchain buffer size
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) {
      imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    swapChainSize = imageCount;

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = surface;
    swapChainCreateInfo.minImageCount = imageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1;
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = context->queueIndices;
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                     indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
      swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      swapChainCreateInfo.queueFamilyIndexCount = 2;
      swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
      swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      swapChainCreateInfo.queueFamilyIndexCount = 0;      // Optional
      swapChainCreateInfo.pQueueFamilyIndices = nullptr;  // Optional
    }
    swapChainCreateInfo.preTransform =
        swapChainSupport.capabilities.currentTransform;
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.clipped = VK_TRUE;
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(context->device, &swapChainCreateInfo, nullptr,
                             &swapChain) != VK_SUCCESS) {
      throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(context->device, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(context->device, swapChain, &imageCount,
                            swapChainImages.data());

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;
    swapChainPresentMode = presentMode;

    lastX = swapChainExtent.width / 2;
    lastY = swapChainExtent.width / 2;
  }

  void destorySwapChain() {
    vkDestroySwapchainKHR(context->device, swapChain, nullptr);
  }

  //   void createSwapChainDepthResources() {
  //     VkFormat depthFormat = findDepthFormat();
  //     gpuRManager->createImage(
  //         defaultWindowContext->swapChainExtent.width,
  //         defaultWindowContext->swapChainExtent.height, 1, depthFormat,
  //         VK_IMAGE_TILING_OPTIMAL,
  //         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
  //         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  //         defaultWindowContext->depthImage,
  //         defaultWindowContext->depthImageMemory);
  //     defaultWindowContext->depthImageView =
  //         gpuRManager->createImageView(defaultWindowContext->depthImage,
  //                                      depthFormat,
  //                                      VK_IMAGE_ASPECT_DEPTH_BIT, 1);
  //     gpuRManager->transitionImageLayout(
  //         defaultWindowContext->depthImage, depthFormat,
  //         VK_IMAGE_LAYOUT_UNDEFINED,
  //         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
  //   }

  // VkFormat RenderServer::findSupportedFormat(
  //     const std::vector<VkFormat> &candidates, VkImageTiling tiling,
  //     VkFormatFeatureFlags features) {
  //   for (VkFormat format : candidates) {
  //     VkFormatProperties props;
  //     vkGetPhysicalDeviceFormatProperties(vkContext->physicalDevice, format,
  //     &props); if (tiling == VK_IMAGE_TILING_LINEAR &&
  //         (props.linearTilingFeatures & features) == features) {
  //       return format;
  //     } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
  //                (props.optimalTilingFeatures & features) == features) {
  //       return format;
  //     }
  //   }
  //   return VK_FORMAT_UNDEFINED;
  // }

  // VkFormat RenderServer::findDepthFormat() {
  //   return findSupportedFormat(
  //       {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
  //        VK_FORMAT_D24_UNORM_S8_UINT},
  //       VK_IMAGE_TILING_OPTIMAL,
  //       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  // }
};
