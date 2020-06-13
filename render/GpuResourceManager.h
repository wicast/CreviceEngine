#ifndef MYVK_GPU_RESOURCEMANAGER_H
#define MYVK_GPU_RESOURCEMANAGER_H 1

#include <map>

#include "common/Resource.h"
#include "render/Context.h"
#include "render/ShaderPack.h"
#include "render/Texture.h"

class GpuResourceManager {
 private:
 public:
  VkContext vkContext;

  // TODO:conbine
  std::map<RID, VkBuffer> uniformBuffers;
  std::map<RID, VkDeviceMemory> uniformBuffersMemory;

  std::map<RID, VkFramebuffer> swapChainFramebuffers;

  // TODO
  VkDescriptorSetLayout descriptorSetLayout;
  VkPipelineLayout pipelineLayout;
  VkRenderPass renderPass;
  VkPipeline graphicsPipeline;

  // TODO
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;

  std::map<RID, myvk::ShaderPack> shaders;
  std::map<RID, myvk::MyTexture> textures;

  void initManager(VkContext vkContext);

  RID addTexture(myvk::MyTexture tex) {
    RID rid = rand();
    textures.insert(std::pair<RID, myvk::MyTexture>(rid, tex));
    return rid;
  }

  RID addShaderPack(myvk::ShaderPack shader) {
    RID rid = rand();
    shaders.insert(std::pair<RID, myvk::ShaderPack>(rid, shader));
    return rid;
  }

  myvk::ShaderPack getShaderPack(RID rid) { return shaders.find(rid)->second; }

  void destoryShaderPack(RID rid);
  RID createShaderPack(const std::string& vertPath,
                       const std::string& fragPath);
  VkShaderModule createShaderModule(const std::vector<char>& code);

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
};

#endif