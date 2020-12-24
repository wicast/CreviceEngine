/**
 * @file GpuResourceManager.h
 * @author wicast (wicast@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-26
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include <shared_mutex>

#include "common/Resource.h"
#include "Context.h"
#include "FrameResource.h"
#include "ShaderPack.h"
#include "resource/VkMesh.hpp"
#include "resource/VkTexture.hpp"
#include "resource/Model.h"
#include "stl/CreviceHashMap.h"
#include "stl/CreviceSharedPtr.h"
#include "stl/CreviceVector.h"

typedef eastl::vector<VkDescriptorSet> DescriptorSets;
typedef eastl::vector<VkCommandBuffer> CommandBuffers;

class GpuResourceManager {
 private:
 public:
  VkContext* vkContext;

  // static uint8_t swapChainSize;

  crevice::HashMap<RID, CommandBuffers> commandBuffers;

  // TODO:conbine
  // std::unordered_map<RID, VkBuffer> uniformBuffers;
  // std::unordered_map<RID, VkDeviceMemory> uniformBuffersMemory;
  // std::unordered_map<RID, VkFramebuffer> swapChainFramebuffers;

  // TODO manage these resource
  // VkPipelineLayout pipelineLayout;
  // VkRenderPass renderPass;
  // VkPipeline graphicsPipeline;

  crevice::Vector<VkDescriptorPool> descriptorPools;
  // crevice::HashMap<RID, VkDescriptorSetLayout> descriptorSetLayouts;
  crevice::HashMap<RID, DescriptorSets> descriptors;

  crevice::HashMap<RID, crevice::SharedPtr<VkMesh>> meshes;
  mutable std::shared_mutex meshMutex;
  // crevice::HashMap<RID, crevice::ShaderPack> shaders;
  crevice::HashMap<RID, eastl::shared_ptr<crevice::VkTexture>> textures;
  mutable std::shared_mutex texMutex;

  void initManager(VkContext* vkContext);

  crevice::SharedPtr<VkRenderPass> createRenderPass(
      VkRenderPassCreateInfo info) {
    VkRenderPass renderPass;
    if (vkCreateRenderPass(vkContext->device, &info, nullptr, &renderPass) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed create render pass");
    }

    return crevice::make_shared<VkRenderPass>(renderPass);
  }

  VkFramebuffer createFramebuffer(VkFramebufferCreateInfo fbInfo) {
    VkFramebuffer fb;
    if (vkCreateFramebuffer(vkContext->device, &fbInfo, nullptr, &fb) !=
        VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
    return fb;
  }

  crevice::FrameResource<VkCommandBuffer> createCommandBuffers(
      uint8_t swapSize) {
    crevice::Vector<VkCommandBuffer> commandBuffers(swapSize);
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkContext->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(vkContext->device, &allocInfo,
                                 commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
    return crevice::FrameResource<VkCommandBuffer>(commandBuffers);
  }

  crevice::FrameResource<VkImageView> createFRImageView(
      crevice::Vector<VkImageView> imgViews) {
    return crevice::FrameResource<VkImageView>(imgViews);
  };

  crevice::FrameResource<VkFramebuffer> createFRFramebuffer(
      crevice::Vector<VkFramebufferCreateInfo> fbInfo) {
    uint8_t swapChainSize = fbInfo.size();
    auto Frfb = crevice::FrameResource<VkFramebuffer>(true, swapChainSize);

    for (size_t i = 0; i < swapChainSize; i++) {
      auto fbr = Frfb.getForUpdate(i);
      auto cinfo = fbInfo[i];
      *fbr = crevice::make_shared<VkFramebuffer>(createFramebuffer(cinfo));
    }

    return Frfb;
  }

  crevice::FrameResource<crevice::VkTexture> createFrameResourceEmptyTexture(
      uint32_t texWidth,
      uint32_t texHeight,
      VkImageUsageFlags usage,
      uint32_t swapSize) {
    auto fRes = crevice::FrameResource<crevice::VkTexture>(true, swapSize);

    for (size_t i = 0; i < swapSize; i++) {
      auto tex = fRes.getForUpdate(i);
      *tex = crevice::make_shared<crevice::VkTexture>(
          createEmptyTexture(texWidth, texHeight, usage));
    }
    return fRes;
  }

  crevice::VkTexture createEmptyTexture(uint32_t texWidth,
                                        uint32_t texHeight,
                                        VkImageUsageFlags usage) {
    crevice::VkTexture newTex{};

    auto mipLevels = 1;
    newTex.height = texWidth;
    newTex.width = texHeight;
    newTex.mipLevels = mipLevels;
    createImage(texWidth, texHeight, mipLevels, VK_FORMAT_R8G8B8A8_UNORM,
                VK_IMAGE_TILING_OPTIMAL, usage,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, newTex.textureImage,
                newTex.textureImageMemory);

    newTex.textureImageView =
        createImageView(newTex.textureImage, VK_FORMAT_R8G8B8A8_UNORM,
                        VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);

    // TODO some image like depth don't need sampler sometime
    createTextureSampler(mipLevels, newTex.textureSampler);

    return newTex;
  }

  // Mesh createMeshFromObj(std::string modelPath);
  // RID addModel(std::string modelPath);
  // RID generateVkMeshBuffer(RID rid);
  void destroyMesh(RID rid);

  // RID addShaderPack(crevice::ShaderPack shader) {
  //   RID rid = 11;
  //   shaders.insert(std::pair<RID,crevice::ShaderPack>(rid,shader));
  //   return rid;
  // }

  void destroyShaderPack(crevice::ShaderPack sp);
  crevice::SharedPtr<crevice::ShaderPack> createShaderPack(
      const std::string& vertPath,
      const std::string& fragPath);
  VkShaderModule createShaderModule(const std::vector<char>& code);

  RID createMyTexture(std::string path);
  void destroyTexture(RID rid);

  void createImage(uint32_t width,
                   uint32_t height,
                   uint32_t mipLevels,
                   VkFormat format,
                   VkImageTiling tiling,
                   VkImageUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkImage& image,
                   VkDeviceMemory& imageMemory);

  VkImageView createImageView(VkImage image,
                              VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);

  void createSwapChainImageViews(VkWindowContext& windowContext,
                                 uint32_t mipLevels = 1);

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);


  void createBuffer(VkDeviceSize size,
                    VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties,
                    VkBuffer& buffer,
                    VkDeviceMemory& bufferMemory);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  void createTextureSampler(uint32_t mipLevels, VkSampler& obj1TextureSampler);

  void transitionImageLayout(VkImage image,
                             VkFormat format,
                             VkImageLayout oldLayout,
                             VkImageLayout newLayout,
                             uint32_t mipLevels);

  void copyBufferToImage(VkBuffer buffer,
                         VkImage image,
                         uint32_t width,
                         uint32_t height);

  void generateMipmaps(VkImage image,
                       VkFormat imageFormat,
                       int32_t texWidth,
                       int32_t texHeight,
                       uint32_t mipLevels);

  inline VkDescriptorPoolSize createDescriptorPoolSize(
      VkDescriptorType type,
      uint32_t descriptorCount) {
    VkDescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.type = type;
    descriptorPoolSize.descriptorCount = descriptorCount;
    return descriptorPoolSize;
  }

  void initDescriptorPool();
  void addDescriptorPool(uint32_t setCount,
                         std::vector<VkDescriptorPoolSize> poolSizes);

  crevice::SharedPtr<VkDescriptorSetLayout> addDescriptorSetLayout(
      VkDescriptorSetLayoutCreateInfo layoutInfo);

  // TODO auto recycle resource 
  void destoryDescriptorSetLayout(VkDescriptorSetLayout layout) {
    // VkDescriptorSetLayout layout
    vkDestroyDescriptorSetLayout(vkContext->device, layout, nullptr);
    // descriptorSetLayouts.erase(rid);
  }

  RID createDescriptorSets(uint32_t swapChainSize,
                           VkDescriptorSetLayout descriptorSetLayout,
                           crevice::Vector<VkBuffer> uniformBuffers,
                           std::vector<RID> imageIds);

  crevice::FrameResource<VkDescriptorSet> createFRDescriptorSet(uint8_t swapChainSize,
      VkDescriptorSetLayout layout,
      crevice::Vector<VkBuffer> buffers = {},
      VkDeviceSize bufferBlockSize = 0,
      crevice::Vector<crevice::VkTexture> images = {});

  RID createIndexedDrawCommandBuffers(
      VkWindowContext windowContext,
      RID meshObjId,
      RID descriptorSets,
      VkRenderPass renderPass,
      VkPipeline graphicsPipeline,
      VkPipelineLayout pipelineLayout,
      std::vector<VkFramebuffer> swapChainFramebuffers,
      VkClearColorValue clearColor = {0.250f, 0.235f, 0.168f, 1.0f});

  void destoryCommandBuffers(RID rid) {
    auto commandBuff = getById<CommandBuffers>(rid);
    vkFreeCommandBuffers(vkContext->device, vkContext->commandPool,
                         static_cast<uint32_t>(commandBuff.size()),
                         commandBuff.data());
    commandBuffers.erase(rid);
  }

  crevice::SharedPtr<crevice::VkTexture> addVkTexture(RID rid,
                                                      crevice::VkTexture tex) {
    std::unique_lock lock(texMutex);
    auto m = eastl::make_shared<crevice::VkTexture>(tex);
    textures.emplace(rid, m);
    return m;
  }

  /**
   * @brief garbage collection
   * TODO
   */ 
  void gc();

  template <class T>
  eastl::shared_ptr<T> getOrNullById(RID rid){};

  template <>
  eastl::shared_ptr<VkMesh> getOrNullById(RID rid) {
    std::shared_lock lock(meshMutex);
    if (meshes.count(rid) == 0) {
      return nullptr;
    }
    return meshes[rid];
  }

  template <>
  eastl::shared_ptr<crevice::VkTexture> getOrNullById(RID rid) {
    std::shared_lock lock(texMutex);
    if (textures.count(rid) == 0) {
      return nullptr;
    }
    return textures[rid];
  }

  template <class T>
  eastl::shared_ptr<T> getOrNew(RID rid){};

  template <>
  eastl::shared_ptr<VkMesh> getOrNew<VkMesh>(RID rid) {
    auto m = getOrNullById<VkMesh>(rid);
    if (m != nullptr) {
      return m;
    }
    std::unique_lock lock(meshMutex);
    m = eastl::make_shared<VkMesh>(*(new VkMesh));
    meshes[rid] = m;
    return m;
  }

  template <>
  eastl::shared_ptr<crevice::VkTexture> getOrNew<crevice::VkTexture>(RID rid) {
    auto m = getOrNullById<crevice::VkTexture>(rid);
    if (m != nullptr) {
      return m;
    }
    std::shared_lock lock(texMutex);
    m = eastl::make_shared<crevice::VkTexture>(*(new crevice::VkTexture));
    textures[rid] = m;
    return m;
  }

  /* getter for resources */
  // TODO error: explicit specialization in non-namespace scope
  template <typename T>
  T getById(RID rid){};

  // template <>
  // crevice::ShaderPack getById<crevice::ShaderPack>(RID rid) {
  //   return shaders.at(rid);
  // }

  template <>
  crevice::SharedPtr<VkMesh> getById<crevice::SharedPtr<VkMesh>>(RID rid) {
    std::shared_lock lock(meshMutex);
    return meshes.at(rid);
  }

  template <>
  eastl::shared_ptr<crevice::VkTexture>
  getById<eastl::shared_ptr<crevice::VkTexture>>(RID rid) {
    std::shared_lock lock(texMutex);
    return textures.at(rid);
  }

  // template <>
  // VkMesh getById<VkMesh>(RID rid) {
  //   return meshes.at(rid);
  // }

  // template <>
  // VkDescriptorSetLayout getById<VkDescriptorSetLayout>(RID rid) {
  //   return descriptorSetLayouts.at(rid);
  // }

  template <>
  DescriptorSets getById<DescriptorSets>(RID rid) {
    return descriptors.at(rid);
  }

  template <>
  CommandBuffers getById<CommandBuffers>(RID rid) {
    return commandBuffers.at(rid);
  }
};
