/**
 * @file Context.h
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
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "containers/Container.h"
#include "utils/MutexUtil.hpp"
#include "volk_imp.h"
#include "stl/CreviceSTL.h"
// #include "vulkan/windowContext.h"
// #include "containers/glfw/glfwContainerImpl.h"

// TODO more configureable
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

/**
 * @brief
 *
 */
struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;
  std::optional<uint32_t> transferFamily;

  // TODO check with user config
  bool isPresentable() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

/**
 * @brief
 *
 */
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

/**
 * @brief
 *
 */
class vkUtil {
 private:
  /* data */
 public:
  static void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT& createInfo,
      PFN_vkDebugUtilsMessengerCallbackEXT debugCallback) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
  }

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void* pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  static bool checkValidationLayerSupport(
      const std::vector<const char*> validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  static bool hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           format == VK_FORMAT_D24_UNORM_S8_UINT;
  }

  static std::vector<const char*> getRequiredExtensions(
      bool enableValidationLayers, const char** glfwExtensions,
      uint32_t glfwExtensionCount) {
    std::vector<const char*> extensions(glfwExtensions,
                                        glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

    return extensions;
  }

  static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device,
                                                       VkSurfaceKHR surface) {
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                              &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         nullptr);
    if (formatCount != 0) {
      details.formats.resize(formatCount);
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                           details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface,
                                              &presentModeCount, nullptr);
    if (presentModeCount != 0) {
      details.presentModes.resize(presentModeCount);
      vkGetPhysicalDeviceSurfacePresentModesKHR(
          device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
  }

  static bool checkDeviceExtensionSupport(
      VkPhysicalDevice device,
      const std::vector<const char*> deviceExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                         availableExtensions.data());
    std::set<std::string> requiredExtension(deviceExtensions.begin(),
                                            deviceExtensions.end());
    for (const auto& extension : availableExtensions) {
      requiredExtension.erase(extension.extensionName);
    }

    return requiredExtension.empty();
  }

  // static bool isDeviceSuitable(
  //     VkPhysicalDevice device,
  //     VkSurfaceKHR surface,
  //     const std::vector<const char*> deviceExtensions) {
  //   QueueFamilyIndices indices = findQueueFamilies(device, surface);
  //   bool extensionsSupported =
  //       checkDeviceExtensionSupport(device, deviceExtensions);

  //   bool swapChainAdequate = false;
  //   if (extensionsSupported) {
  //     SwapChainSupportDetails swapChainSupport =
  //         querySwapChainSupport(device, surface);
  //     swapChainAdequate = !swapChainSupport.formats.empty() &&
  //                         !swapChainSupport.presentModes.empty();
  //   }
  //   VkPhysicalDeviceFeatures supportedFeatures;
  //   vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
  //   return indices.isComplete() && extensionsSupported && swapChainAdequate
  //   &&
  //          supportedFeatures.samplerAnisotropy;
  // }

  static VkResult CreateDebugUtilsMessengerEXT(
      VkInstance instance,
      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
      const VkAllocationCallbacks* pAllocator,
      VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  static void DestroyDebugUtilsMessengerEXT(
      VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
      const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
      func(instance, debugMessenger, pAllocator);
    }
  }

  // Swapchain utils
  static VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
          availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
      }
    }
    return availableFormats[0];
  }

  static VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
        return VK_PRESENT_MODE_FIFO_RELAXED_KHR;
      }
    }

    return VK_PRESENT_MODE_MAILBOX_KHR;
  }
};

class VkContext {
 public:
  VkInstance instance;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  QueueFamilyIndices queueIndices;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  // TODO deal with device that only has 1 QueueCount
  VkQueue singleOpQueue;
  bool sameQueueGraphicAndOp = true;
  // TODO for now just make sure there is only one thread operating, multi
  // thread submition should used in future
  priorityMutex singleOpQueueLock;
  VkFence singleOpFence;

  // TODO set into feature info
  // TODO hardcode for testing
  static eastl::vector<size_t> queueFamCount;

  //TODO multi commandpool
  VkCommandPool commandPool;
  VkCommandPool singleTimeOpCommandPool;

  bool deviceEnabled;
  bool enableValidationLayers;
  VkDebugUtilsMessengerEXT debugMessenger;

  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice pDevice,
                                       VkSurfaceKHR surface) {
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(pDevice, &queueFamilyCount,
                                             queueFamilies.data());
    int i = 0;
    queueFamCount.resize(queueFamilies.size());
    for (const auto& queueFamily : queueFamilies) {
      queueFamCount[i] = queueFamily.queueCount;
      if (queueFamily.queueCount > 0 &&
          queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices.graphicsFamily = i;
      }
      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(pDevice, i, surface,
                                           &presentSupport);
      if (queueFamily.queueCount > 0 && presentSupport) {
        indices.presentFamily = i;
      }
      if (indices.isPresentable()) {
        break;
      }

      i++;
    }
    return indices;
  }

  // void createSwapChain(crevice::Container& container,
  //                      VkWindowContext& windowContext) {
  //   SwapChainSupportDetails swapChainSupport =
  //       vkUtil::querySwapChainSupport(physicalDevice, windowContext.surface);

  //   VkSurfaceFormatKHR surfaceFormat =
  //       vkUtil::chooseSwapSurfaceFormat(swapChainSupport.formats);
  //   VkPresentModeKHR presentMode =
  //       vkUtil::chooseSwapPresentMode(swapChainSupport.presentModes);
  //   VkExtent2D extent =
  //       chooseSwapExtent(swapChainSupport.capabilities, &container);

  //   // TODO select swapchain buffer size
  //   uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  //   if (swapChainSupport.capabilities.maxImageCount > 0 &&
  //       imageCount > swapChainSupport.capabilities.maxImageCount) {
  //     imageCount = swapChainSupport.capabilities.maxImageCount;
  //   }

  //   windowContext.swapChainSize = imageCount;

  //   VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
  //   swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  //   swapChainCreateInfo.surface = windowContext.surface;
  //   swapChainCreateInfo.minImageCount = imageCount;
  //   swapChainCreateInfo.imageFormat = surfaceFormat.format;
  //   swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  //   swapChainCreateInfo.imageExtent = extent;
  //   swapChainCreateInfo.imageArrayLayers = 1;
  //   swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  //   QueueFamilyIndices indices = queueIndices;
  //   uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
  //                                    indices.presentFamily.value()};

  //   if (indices.graphicsFamily != indices.presentFamily) {
  //     swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
  //     swapChainCreateInfo.queueFamilyIndexCount = 2;
  //     swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  //   } else {
  //     swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  //     swapChainCreateInfo.queueFamilyIndexCount = 0;      // Optional
  //     swapChainCreateInfo.pQueueFamilyIndices = nullptr;  // Optional
  //   }
  //   swapChainCreateInfo.preTransform =
  //       swapChainSupport.capabilities.currentTransform;
  //   swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  //   swapChainCreateInfo.presentMode = presentMode;
  //   swapChainCreateInfo.clipped = VK_TRUE;
  //   swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  //   if (vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr,
  //                            &windowContext.swapChain) != VK_SUCCESS) {
  //     throw std::runtime_error("failed to create swap chain!");
  //   }

  //   vkGetSwapchainImagesKHR(device, windowContext.swapChain, &imageCount,
  //                           nullptr);
  //   windowContext.swapChainImages.resize(imageCount);
  //   vkGetSwapchainImagesKHR(device, windowContext.swapChain, &imageCount,
  //                           windowContext.swapChainImages.data());

  //   windowContext.swapChainImageFormat = surfaceFormat.format;
  //   windowContext.swapChainExtent = extent;
  //   windowContext.swapChainPresentMode = presentMode;

  //   windowContext.lastX = windowContext.swapChainExtent.width / 2;
  //   windowContext.lastY = windowContext.swapChainExtent.width / 2;
  // }

  bool isDevicePresentable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    auto deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    bool extensionsSupported =
        vkUtil::checkDeviceExtensionSupport(device, deviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
      SwapChainSupportDetails swapChainSupport =
          vkUtil::querySwapChainSupport(device, surface);
      swapChainAdequate = !swapChainSupport.formats.empty() &&
                          !swapChainSupport.presentModes.empty();
    }
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    return indices.isPresentable() && extensionsSupported &&
           swapChainAdequate && supportedFeatures.samplerAnisotropy;
  }

  void createInstance(std::vector<const char*> requiredExtensions) {
    if (enableValidationLayers &&
        !vkUtil::checkValidationLayerSupport(validationLayers)) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Crevice Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Crevice Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    uint32_t extensionCount = 0;
    // get extension Count
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());
    std::cout << "available extensions:" << std::endl;

    for (const auto& extension : extensions) {
      std::cout << "\t" << extension.extensionName << std::endl;
    }

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
      instanceCreateInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
      vkUtil::populateDebugMessengerCreateInfo(debugCreateInfo,
                                               vkUtil::debugCallback);
      instanceCreateInfo.pNext =
          (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
      instanceCreateInfo.enabledLayerCount = 0;
      instanceCreateInfo.pNext = nullptr;
    }

    instanceCreateInfo.enabledExtensionCount = requiredExtensions.size();
    instanceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();

    instanceCreateInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
  }

  void pickPhysicalDevice(VkSurfaceKHR surface) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
      throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
      if (isDevicePresentable(device, surface)) {
        physicalDevice = device;
        queueIndices = findQueueFamilies(device, surface);
        break;
      }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
      throw std::runtime_error("failed to find a suitable GPU!");
    }
  }

  void createLogicalDeviceAndQueue() {
    QueueFamilyIndices indices = queueIndices;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                              indices.presentFamily.value()};

    float queuePriority = 1.0f;
    // TODO smart select queue for graphic and transfer
    for (uint32_t queueFamily : uniqueQueueFamilies) {
      VkDeviceQueueCreateInfo queueCreateInfo = {};
      queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      queueCreateInfo.queueFamilyIndex = queueFamily;

      auto qcount = 1;
      if (queueFamCount[queueFamily] > 1) {
        // HACK hardcoded number of queue number in a family
        qcount = 2;
      }
      queueCreateInfo.queueCount = qcount;
      queueCreateInfo.pQueuePriorities = &queuePriority;
      queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
    deviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount =
        static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create logical device!");
    }

    // TODO do smart auto config, hardcode for now
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    auto singleQIdx = 0;
    if (queueFamCount[indices.graphicsFamily.value()] > 1) {
      singleQIdx = 1;
      sameQueueGraphicAndOp = false;
    }
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), singleQIdx,
                     &singleOpQueue);

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (vkCreateFence(device, &fenceInfo, nullptr, &singleOpFence) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create singleOpFence!");
    }
  }

  // // TODO no binding of glfw
  // VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
  //                             crevice::Container* container) {
  //   if (capabilities.currentExtent.width != UINT32_MAX) {
  //     return capabilities.currentExtent;
  //   } else {
  //     int width, height;
  //     container->getFramebufferSize(&width, &height);

  //     VkExtent2D actualExtent = {static_cast<uint32_t>(width),
  //                                static_cast<uint32_t>(height)};

  //     actualExtent.width = std::max(
  //         capabilities.minImageExtent.width,
  //         std::min(capabilities.maxImageExtent.width, actualExtent.width));
  //     actualExtent.height = std::max(
  //         capabilities.minImageExtent.height,
  //         std::min(capabilities.maxImageExtent.height, actualExtent.height));

  //     return actualExtent;
  //   }
  // }

  void createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = queueIndices;

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfo.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  // Optional

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create command pool!");
    }

    VkCommandPoolCreateInfo poolInfoDataOp = {};
    poolInfoDataOp.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfoDataOp.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    poolInfoDataOp.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;  // Optional

    if (vkCreateCommandPool(device, &poolInfoDataOp, nullptr,
                            &singleTimeOpCommandPool) != VK_SUCCESS) {
      throw std::runtime_error("failed to create command pool!");
    }
  }

  VkCommandBuffer beginSingleTimeCommands() {
    singleOpQueueLock.lockLow();
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = singleTimeOpCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
  }

  void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    vkResetFences(device, 1, &singleOpFence);
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(singleOpQueue, 1, &submitInfo, singleOpFence);
    vkWaitForFences(device, 1, &singleOpFence, VK_TRUE,
                    UINT64_MAX);  // Prevent unfinished work
    vkFreeCommandBuffers(device, singleTimeOpCommandPool, 1, &commandBuffer);
    singleOpQueueLock.unlockLow();
  }

  void setupDebugMessenger() {
    if (!enableValidationLayers) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    vkUtil::populateDebugMessengerCreateInfo(createInfo, vkUtil::debugCallback);

    if (vkUtil::CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                             &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }
};