#ifndef MYVK_GPU_RESOURCEMANAGER_H
#define MYVK_GPU_RESOURCEMANAGER_H 1

#include <map>

#include "common/Resource.h"
#include "render/Context.h"
#include "render/ShaderPack.h"
#include "render/Texture.h"
#include "render/Model.h"

class GpuResourceManager {
 private:
 public:
  VkContext* vkContext;

  // TODO:conbine
  std::map<RID, VkBuffer> uniformBuffers;
  std::map<RID, VkDeviceMemory> uniformBuffersMemory;

  std::map<RID, VkFramebuffer> swapChainFramebuffers;

  // TODO
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkRenderPass renderPass;
  VkPipeline graphicsPipeline;

  std::map<RID, Mesh> meshs;
  std::map<RID, myvk::ShaderPack> shaders;
  std::map<RID, myvk::MyTexture> textures;

  void initManager(VkContext* vkContext);

  Mesh createMeshFromObj(std::string modelPath);
  RID addModel(std::string modelPath);
  RID generateVkMeshBuffer(RID rid);
  Mesh getMesh(RID rid) { return meshs.at(rid); }
  void destoryMesh(RID rid);

  RID addShaderPack(myvk::ShaderPack shader) {
    RID rid = rand();
    shaders.insert(std::pair<RID, myvk::ShaderPack>(rid, shader));
    return rid;
  }

  myvk::ShaderPack getShaderPack(RID rid) { return shaders.at(rid); }

  void destoryShaderPack(RID rid);
  RID createShaderPack(const std::string& vertPath,
                       const std::string& fragPath);
  VkShaderModule createShaderModule(const std::vector<char>& code);

  RID createMyTexture(std::string path);
  myvk::MyTexture getTexture(RID rid) { return textures.at(rid); }
  void destoryTexture(RID rid);

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
};

#endif