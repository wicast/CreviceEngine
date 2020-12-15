#pragma once

#include "volk_imp.h"

struct VkMesh {
  VkDeviceSize vertexSize;
  VkDeviceSize indicesSize;
  VkDeviceSize bufferSize;

  VkBuffer vertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
  VkBuffer indexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

  ~VkMesh();
  // ~VkMesh() {
  // auto server = crevice::RenderServer::getInstance();
  // auto vkContext = server->vkContext;

  // vkDestroyBuffer(vkContext->device, indexBuffer, nullptr);
  // vkFreeMemory(vkContext->device, indexBufferMemory, nullptr);

  // vkDestroyBuffer(vkContext->device, vertexBuffer, nullptr);
  // vkFreeMemory(vkContext->device, vertexBufferMemory, nullptr);
  // }
};
