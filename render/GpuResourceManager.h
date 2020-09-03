#ifndef MYVK_GPU_RESOURCEMANAGER_H
#define MYVK_GPU_RESOURCEMANAGER_H 1

#include <unordered_map>

#include "common/Resource.h"
#include "render/Context.h"
#include "render/Model.h"
#include "render/ShaderPack.h"
#include "render/Texture.h"

#include "EASTL/hash_map.h"
#include "EASTL/vector.h"

typedef std::vector<VkDescriptorSet> DescriptorSets;
typedef std::vector<VkCommandBuffer> CommandBuffers;

class GpuResourceManager {
 private:
 public:
  VkContext* vkContext;

  eastl::hash_map<RID, CommandBuffers> commandBuffers;

  // TODO:conbine
  // std::unordered_map<RID, VkBuffer> uniformBuffers;
  // std::unordered_map<RID, VkDeviceMemory> uniformBuffersMemory;
  // std::unordered_map<RID, VkFramebuffer> swapChainFramebuffers;

  // TODO
  // VkPipelineLayout pipelineLayout;
  // VkRenderPass renderPass;
  // VkPipeline graphicsPipeline;

  eastl::vector<VkDescriptorPool> descriptorPools;
  eastl::hash_map<RID, VkDescriptorSetLayout> descriptorSetLayouts;
  eastl::hash_map<RID, DescriptorSets> descriptors;

  eastl::hash_map<RID, Mesh> meshs;
  eastl::hash_map<RID, myvk::ShaderPack> shaders;
  eastl::hash_map<RID, myvk::MyTexture> textures;

  void initManager(VkContext* vkContext);

  Mesh createMeshFromObj(std::string modelPath);
  RID addModel(std::string modelPath);
  RID generateVkMeshBuffer(RID rid);
  void destroyMesh(RID rid);

  RID addShaderPack(myvk::ShaderPack shader) {
    RID rid = rand();
    shaders.emplace(rid, shader);
    return rid;
  }

  void destroyShaderPack(RID rid);
  RID createShaderPack(const std::string& vertPath,
                       const std::string& fragPath);
  VkShaderModule createShaderModule(const std::vector<char>& code);

  RID createMyTexture(std::string path);
  void destroyTexture(RID rid);

  void createImage(uint32_t width, uint32_t height, uint32_t mipLevels,
                   VkFormat format, VkImageTiling tiling,
                   VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
                   VkImage& image, VkDeviceMemory& imageMemory);

  VkImageView createImageView(VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags,
                              uint32_t mipLevels);

  void createSwapChainImageViews(WindowContext& windowContext,
                                 uint32_t mipLevels = 1);

  uint32_t findMemoryType(uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                              GLFWwindow* window);
  void createSwapChain(WindowContext& windowContext);

  void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties, VkBuffer& buffer,
                    VkDeviceMemory& bufferMemory);

  void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  void createTextureSampler(uint32_t mipLevels, VkSampler& obj1TextureSampler);

  void transitionImageLayout(VkImage image, VkFormat format,
                             VkImageLayout oldLayout, VkImageLayout newLayout,
                             uint32_t mipLevels);

  void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width,
                         uint32_t height);

  void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth,
                       int32_t texHeight, uint32_t mipLevels);

  inline VkDescriptorPoolSize createDescriptorPoolSize(
      VkDescriptorType type, uint32_t descriptorCount) {
    VkDescriptorPoolSize descriptorPoolSize{};
    descriptorPoolSize.type = type;
    descriptorPoolSize.descriptorCount = descriptorCount;
    return descriptorPoolSize;
  }

  void initDescriptorPool();
  void addDescriptorPool(uint32_t setCount,
                         std::vector<VkDescriptorPoolSize> poolSizes);

  RID addDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo layoutInfo);

  void destoryDescriptorSetLayout(RID rid) {
    VkDescriptorSetLayout layout = getById<VkDescriptorSetLayout>(rid);
    vkDestroyDescriptorSetLayout(vkContext->device, layout, nullptr);
    descriptorSetLayouts.erase(rid);
  }

  RID createDescriptorSets(uint32_t swapChainSize, RID descriptorSetLayoutId,
                           std::vector<VkBuffer> uniformBuffers,
                           std::vector<RID> imageIds);

  RID createIndexedDrawCommandBuffers(
      WindowContext windowContext, RID meshObjId, RID descriptorSets,
      VkRenderPass renderPass, VkPipeline graphicsPipeline,
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

  /* getter for resources */
  template <typename T>
  T getById(RID rid){};

  template <>
  myvk::ShaderPack getById<myvk::ShaderPack>(RID rid) {
    return shaders.at(rid);
  }

  template <>
  myvk::MyTexture getById<myvk::MyTexture>(RID rid) {
    return textures.at(rid);
  }

  template <>
  Mesh getById<Mesh>(RID rid) {
    return meshs.at(rid);
  }

  template <>
  VkDescriptorSetLayout getById<VkDescriptorSetLayout>(RID rid) {
    return descriptorSetLayouts.at(rid);
  }

  template <>
  DescriptorSets getById<DescriptorSets>(RID rid) {
    return descriptors.at(rid);
  }

  template <>
  CommandBuffers getById<CommandBuffers>(RID rid) {
    return commandBuffers.at(rid);
  }
};

#endif