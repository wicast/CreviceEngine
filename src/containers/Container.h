/**
 * @file Container.h
 * @author wicast (wicast@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include <stdexcept>

#include "volk_imp.h"

namespace crevice {
/**
 * @brief Window Container
 *
 */
class Container {
 protected:
 private:
  /* data */

 public:
  void createWindow(int width, int height, const char* title);
  void createSurface(VkInstance instance, VkSurfaceKHR* surface,
                     const VkAllocationCallbacks* allocator = nullptr);
  void getFramebufferSize(int* width, int* height);
  const char** getVkRequiredInstanceExtensions(uint32_t* count);
};

}  // namespace crevice
