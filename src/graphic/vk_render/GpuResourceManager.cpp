// #define TINYOBJLOADER_IMPLEMENTATION
// #include <tiny_obj_loader.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#include "GpuResourceManager.h"
#include "FrameResource.h"
#include "Uniform.h"
#include "resource/Model.h"

// TODO move to cpu resource
// Mesh GpuResourceManager::createMeshFromObj(std::string modelPath) {
//   tinyobj::attrib_t attrib;
//   std::vector<tinyobj::shape_t> shapes;
//   std::vector<tinyobj::material_t> materials;
//   std::string warn, err;

//   Mesh mesh;

//   if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
//                         modelPath.c_str())) {
//     throw std::runtime_error(warn + err);
//   }

//   std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

//   for (const auto& shape : shapes) {
//     for (const auto& index : shape.mesh.indices) {
//       Vertex vertex = {};
//       vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
//                     attrib.vertices[3 * index.vertex_index + 1],
//                     attrib.vertices[3 * index.vertex_index + 2]};

//       vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
//                          1.0f - attrib.texcoords[2 * index.texcoord_index +
//                          1]};

//       vertex.normal = {attrib.normals[3 * index.normal_index + 0],
//                        attrib.normals[3 * index.normal_index + 1],
//                        attrib.normals[3 * index.normal_index + 2]};

//       vertex.color = {1.0f, 1.0f, 1.0f};
//       if (uniqueVertices.count(vertex) == 0) {
//         uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
//         mesh.vertices.push_back(vertex);
//       }

//       mesh.indices.push_back(uniqueVertices[vertex]);
//     }
//   }

//   return mesh;
// }

// RID GpuResourceManager::addModel(std::string modelPath) {
//   // TODO beyond obj type
//   Mesh newMesh = createMeshFromObj(modelPath);
//   RID rid = rand();
//   this->meshes.emplace(rid, newMesh);

//   return rid;
// }

// RID GpuResourceManager::generateVkMeshBuffer(RID rid) {
//   // VertexBuffer
//   //TODO from resource manager
//   // auto mesh = getById<Mesh>(rid);
//   Mesh mesh;

//   VkMesh vkMesh{};

//   vkMesh.indicesSize = mesh.indices.size();
//   vkMesh.vertexSize = mesh.vertices.size();
//   VkDeviceSize bufferSize = sizeof(Vertex) * vkMesh.vertexSize;
//   vkMesh.bufferSize = bufferSize;

//   VkBuffer stagingBuffer = VK_NULL_HANDLE;
//   VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

//   createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                stagingBuffer, stagingBufferMemory);

//   void* data;
//   vkMapMemory(vkContext->device, stagingBufferMemory, 0, bufferSize, 0,
//   &data); memcpy(data, mesh.vertices.data(), (size_t)bufferSize);
//   vkUnmapMemory(vkContext->device, stagingBufferMemory);

//   createBuffer(
//       bufferSize,
//       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkMesh.vertexBuffer,
//       vkMesh.vertexBufferMemory);

//   copyBuffer(stagingBuffer, vkMesh.vertexBuffer, bufferSize);

//   vkDestroyBuffer(vkContext->device, stagingBuffer, nullptr);
//   vkFreeMemory(vkContext->device, stagingBufferMemory, nullptr);

//   // IndexBuffer
//   bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();

//   createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                stagingBuffer, stagingBufferMemory);

//   vkMapMemory(vkContext->device, stagingBufferMemory, 0, bufferSize, 0,
//   &data); memcpy(data, mesh.indices.data(), (size_t)bufferSize);
//   vkUnmapMemory(vkContext->device, stagingBufferMemory);

//   createBuffer(
//       bufferSize,
//       VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
//       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkMesh.indexBuffer,
//       vkMesh.indexBufferMemory);

//   copyBuffer(stagingBuffer, vkMesh.indexBuffer, bufferSize);

//   vkDestroyBuffer(vkContext->device, stagingBuffer, nullptr);
//   vkFreeMemory(vkContext->device, stagingBufferMemory, nullptr);

//   meshes.emplace(rid, vkMesh);

//   return rid;
// }

void GpuResourceManager::destroyMesh(RID rid) {
  // auto mesh = getById<VkMesh>(rid);
  meshes.erase(rid);
}

crevice::SharedPtr<crevice::ShaderPack> GpuResourceManager::createShaderPack(
    const std::string& vertPath,
    const std::string& fragPath) {
  auto vertShaderCode = readFile(vertPath);
  auto fragShaderCode = readFile(fragPath);

  VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
  VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

  crevice::ShaderPack shaderpack;
  shaderpack.fragShaderModule = fragShaderModule;
  shaderpack.vertShaderModule = vertShaderModule;
  return crevice::make_shared<crevice::ShaderPack>(shaderpack);
}

void GpuResourceManager::initManager(VkContext* vkContext) {
  srand(time(0));
  this->vkContext = vkContext;
}

VkShaderModule GpuResourceManager::createShaderModule(
    const std::vector<char>& code) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(vkContext->device, &createInfo, nullptr,
                           &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}

void GpuResourceManager::destroyShaderPack(crevice::ShaderPack sp) {
  // crevice::ShaderPack sp = getById<crevice::ShaderPack>(rid);

  vkDestroyShaderModule(vkContext->device, sp.fragShaderModule, nullptr);
  vkDestroyShaderModule(vkContext->device, sp.vertShaderModule, nullptr);

  // shaders.erase(rid);
}

void GpuResourceManager::destroyTexture(RID rid) {
  auto tex = *(getById<eastl::shared_ptr<crevice::VkTexture>>(rid));

  vkDestroySampler(vkContext->device, tex.textureSampler, nullptr);
  vkDestroyImageView(vkContext->device, tex.textureImageView, nullptr);
  vkDestroyImage(vkContext->device, tex.textureImage, nullptr);
  vkFreeMemory(vkContext->device, tex.textureImageMemory, nullptr);
  textures.erase(rid);
}

void GpuResourceManager::createImage(uint32_t width,
                                     uint32_t height,
                                     uint32_t mipLevels,
                                     VkFormat format,
                                     VkImageTiling tiling,
                                     VkImageUsageFlags usage,
                                     VkMemoryPropertyFlags properties,
                                     VkImage& image,
                                     VkDeviceMemory& imageMemory) {
  VkImageCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevels;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = usage;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(vkContext->device, &imageInfo, nullptr, &image) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(vkContext->device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(vkContext->device, &allocInfo, nullptr, &imageMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  vkBindImageMemory(vkContext->device, image, imageMemory, 0);
}

void GpuResourceManager::createTextureSampler(uint32_t mipLevels,
                                              VkSampler& obj1TextureSampler) {
  VkSamplerCreateInfo samplerInfo = {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;

  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = 16;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

  samplerInfo.unnormalizedCoordinates = VK_FALSE;

  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

  samplerInfo.unnormalizedCoordinates = VK_FALSE;

  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.minLod = 0;  // Optional
  samplerInfo.maxLod = static_cast<float>(mipLevels);
  samplerInfo.mipLodBias = 0;  // Optional

  if (vkCreateSampler(vkContext->device, &samplerInfo, nullptr,
                      &obj1TextureSampler) != VK_SUCCESS) {
    throw std::runtime_error("failed to create texture sampler!");
  }
}

void GpuResourceManager::createBuffer(VkDeviceSize size,
                                      VkBufferUsageFlags usage,
                                      VkMemoryPropertyFlags properties,
                                      VkBuffer& buffer,
                                      VkDeviceMemory& bufferMemory) {
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(vkContext->device, &bufferInfo, nullptr, &buffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vkContext->device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
      findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(vkContext->device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(vkContext->device, buffer, bufferMemory, 0);
}

void GpuResourceManager::copyBuffer(VkBuffer srcBuffer,
                                    VkBuffer dstBuffer,
                                    VkDeviceSize size) {
  VkCommandBuffer commandBuffer = vkContext->beginSingleTimeCommands();

  VkBufferCopy copyRegion = {};
  copyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

  vkContext->endSingleTimeCommands(commandBuffer);
}

// RID GpuResourceManager::createMyTexture(std::string path) {
//   crevice::VkTexture newTex{};

//   int texWidth, texHeight, texChannels;
//   stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight,
//   &texChannels,
//                               STBI_rgb_alpha);
//   VkDeviceSize imageSize = texWidth * texHeight * 4;
//   int mipLevels = static_cast<uint32_t>(
//                       std::floor(std::log2(std::max(texWidth, texHeight)))) +
//                   1;

//   if (!pixels) {
//     throw std::runtime_error("failed to load texture image!");
//   }
//   newTex.mipLevels = mipLevels;
//   newTex.width = texWidth;
//   newTex.height = texHeight;

//   VkBuffer stagingBuffer;
//   VkDeviceMemory stagingBufferMemory;
//   createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
//                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//                stagingBuffer, stagingBufferMemory);
//   void* data;
//   vkMapMemory(vkContext->device, stagingBufferMemory, 0, imageSize, 0,
//   &data); memcpy(data, pixels, static_cast<size_t>(imageSize));
//   vkUnmapMemory(vkContext->device, stagingBufferMemory);
//   stbi_image_free(pixels);

//   createImage(texWidth, texHeight, mipLevels, VK_FORMAT_R8G8B8A8_UNORM,
//               VK_IMAGE_TILING_OPTIMAL,
//               VK_IMAGE_USAGE_TRANSFER_DST_BIT |
//                   VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
//                   VK_IMAGE_USAGE_SAMPLED_BIT,
//               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, newTex.textureImage,
//               newTex.textureImageMemory);

//   transitionImageLayout(newTex.textureImage, VK_FORMAT_R8G8B8A8_UNORM,
//                         VK_IMAGE_LAYOUT_UNDEFINED,
//                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
//   copyBufferToImage(stagingBuffer, newTex.textureImage,
//                     static_cast<uint32_t>(texWidth),
//                     static_cast<uint32_t>(texHeight));
//   generateMipmaps(newTex.textureImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth,
//                   texHeight, mipLevels);

//   vkDestroyBuffer(vkContext->device, stagingBuffer, nullptr);
//   vkFreeMemory(vkContext->device, stagingBufferMemory, nullptr);

//   newTex.textureImageView =
//       createImageView(newTex.textureImage, VK_FORMAT_R8G8B8A8_UNORM,
//                       VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);

//   // TODO some image like depth don't need sampler sometime
//   createTextureSampler(mipLevels, newTex.textureSampler);

//   RID rid = rand();
//   textures.emplace(rid, newTex);
//   return rid;
// }

void GpuResourceManager::copyBufferToImage(VkBuffer buffer,
                                           VkImage image,
                                           uint32_t width,
                                           uint32_t height) {
  VkCommandBuffer commandBuffer = vkContext->beginSingleTimeCommands();

  VkBufferImageCopy region = {};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {width, height, 1};

  vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  vkContext->endSingleTimeCommands(commandBuffer);
}

void GpuResourceManager::transitionImageLayout(VkImage image,
                                               VkFormat format,
                                               VkImageLayout oldLayout,
                                               VkImageLayout newLayout,
                                               uint32_t mipLevels) {
  VkCommandBuffer commandBuffer = vkContext->beginSingleTimeCommands();

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = oldLayout;
  barrier.newLayout = newLayout;

  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = image;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mipLevels;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    if (vkUtil::hasStencilComponent(format)) {
      barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
  } else {
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;
  if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
      newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
             newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
                       nullptr, 0, nullptr, 1, &barrier);

  vkContext->endSingleTimeCommands(commandBuffer);
}

void GpuResourceManager::generateMipmaps(VkImage image,
                                         VkFormat imageFormat,
                                         int32_t texWidth,
                                         int32_t texHeight,
                                         uint32_t mipLevels) {
  VkFormatProperties formatProperties;
  vkGetPhysicalDeviceFormatProperties(vkContext->physicalDevice, imageFormat,
                                      &formatProperties);
  if (!(formatProperties.optimalTilingFeatures &
        VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
    throw std::runtime_error(
        "texture image format does not support linear blitting!");
  }

  VkCommandBuffer commandBuffer = vkContext->beginSingleTimeCommands();

  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.image = image;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.subresourceRange.levelCount = 1;

  int32_t mipWidth = texWidth;
  int32_t mipHeight = texHeight;

  for (uint32_t i = 1; i < mipLevels; i++) {
    barrier.subresourceRange.baseMipLevel = i - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &barrier);

    VkImageBlit blit = {};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = i - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1,
                          mipHeight > 1 ? mipHeight / 2 : 1, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = i;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                   VK_FILTER_LINEAR);

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);

    if (mipWidth > 1)
      mipWidth /= 2;
    if (mipHeight > 1)
      mipHeight /= 2;
  }
  barrier.subresourceRange.baseMipLevel = mipLevels - 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

  vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                       VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
                       nullptr, 1, &barrier);

  vkContext->endSingleTimeCommands(commandBuffer);
}

// // TODO no binding of glfw
// VkExtent2D GpuResourceManager::chooseSwapExtent(
//     const VkSurfaceCapabilitiesKHR& capabilities,
//     crevice::GLFWContainer* container) {
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

VkImageView GpuResourceManager::createImageView(VkImage image,
                                                VkFormat format,
                                                VkImageAspectFlags aspectFlags,
                                                uint32_t mipLevels) {
  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = image;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  VkImageView imageView;
  if (vkCreateImageView(vkContext->device, &viewInfo, nullptr, &imageView) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create texture image view!");
  }

  return imageView;
}

void GpuResourceManager::createSwapChainImageViews(
    VkWindowContext& windowContext,
    uint32_t mipLevels) {
  windowContext.swapChainImageViews.resize(
      windowContext.swapChainImages.size());
  for (size_t i = 0; i < windowContext.swapChainImages.size(); i++) {
    windowContext.swapChainImageViews[i] = createImageView(
        windowContext.swapChainImages[i], windowContext.swapChainImageFormat,
        VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
  }
}

uint32_t GpuResourceManager::findMemoryType(uint32_t typeFilter,
                                            VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(vkContext->physicalDevice,
                                      &memProperties);

  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
                                    properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

void GpuResourceManager::initDescriptorPool() {
  std::vector<VkDescriptorPoolSize> poolSizes = {
      createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 256),
      createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256)};
  addDescriptorPool(256, poolSizes);
}

void GpuResourceManager::addDescriptorPool(
    uint32_t setCount,
    std::vector<VkDescriptorPoolSize> poolSizes) {
  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(setCount);

  VkDescriptorPool descriptorPool;
  if (vkCreateDescriptorPool(vkContext->device, &poolInfo, nullptr,
                             &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }

  descriptorPools.push_back(descriptorPool);
}

crevice::SharedPtr<VkDescriptorSetLayout>
GpuResourceManager::addDescriptorSetLayout(
    VkDescriptorSetLayoutCreateInfo layoutInfo) {
  VkDescriptorSetLayout descriptorSetLayout{};
  if (vkCreateDescriptorSetLayout(vkContext->device, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }

  crevice::SharedPtr<VkDescriptorSetLayout> ptr =
      crevice::make_shared<VkDescriptorSetLayout>(descriptorSetLayout);
  return ptr;
}

// TODO free layout of descriptor set
RID GpuResourceManager::createDescriptorSets(
    uint32_t swapChainSize,
    VkDescriptorSetLayout descriptorSetLayout,
    crevice::Vector<VkBuffer> uniformBuffers,
    std::vector<RID> imageIds) {
  eastl::vector<VkDescriptorSetLayout> layouts(swapChainSize,
                                               descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPools[0];
  allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainSize);
  allocInfo.pSetLayouts = layouts.data();

  eastl::vector<VkDescriptorSet> descriptorSets(swapChainSize);

  if (vkAllocateDescriptorSets(vkContext->device, &allocInfo,
                               descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < swapChainSize; i++) {
    eastl::vector<VkWriteDescriptorSet> descriptorWrites;

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkWriteDescriptorSet desWriteUbo = {};
    desWriteUbo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    desWriteUbo.dstSet = descriptorSets[i];
    desWriteUbo.dstBinding = 0;
    desWriteUbo.dstArrayElement = 0;
    desWriteUbo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    desWriteUbo.descriptorCount = 1;
    desWriteUbo.pBufferInfo = &bufferInfo;

    descriptorWrites.push_back(desWriteUbo);

    eastl::vector<VkDescriptorImageInfo> imageInfos(imageIds.size());

    for (auto j = 0; j < imageIds.size(); j++) {
      auto tex = *(getById<eastl::shared_ptr<crevice::VkTexture>>(imageIds[j]));

      imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageInfos[j].imageView = tex.textureImageView;
      imageInfos[j].sampler = tex.textureSampler;

      VkWriteDescriptorSet desWriteImg = {};

      desWriteImg.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      desWriteImg.dstSet = descriptorSets[i];
      desWriteImg.dstBinding = j + 1;
      desWriteImg.dstArrayElement = 0;
      desWriteImg.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      desWriteImg.descriptorCount = 1;
      desWriteImg.pImageInfo = &imageInfos[j];

      descriptorWrites.push_back(desWriteImg);
    }

    vkUpdateDescriptorSets(vkContext->device,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);
  }

  RID rid = rand();
  this->descriptors.emplace(rid, descriptorSets);
  return rid;
}

crevice::FrameResource<VkDescriptorSet>
GpuResourceManager::createFRDescriptorSet(uint8_t swapChainSize,
    VkDescriptorSetLayout layout,
    crevice::Vector<VkBuffer> buffers,
    VkDeviceSize bufferBlockSize,
    crevice::Vector<crevice::VkTexture> images) {
  if (!buffers.empty() && !images.empty()) {
    throw std::runtime_error(
        "only create a buffer or image descriptor once a time");
  } else if (buffers.empty() && images.empty()) {
    throw std::runtime_error(
        "at least a buffer or image descriptor need to be created");
  } else if (bufferBlockSize == 0 && !buffers.empty() ||
             (bufferBlockSize != 0 && buffers.empty())) {
    throw std::runtime_error("buffer create parameter error");
  }

  using namespace crevice;
  Vector<VkDescriptorSetLayout> layouts(swapChainSize, layout);
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPools[0];
  allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainSize);
  allocInfo.pSetLayouts = layouts.data();

  Vector<VkDescriptorSet> descriptorSets(swapChainSize);

  if (vkAllocateDescriptorSets(vkContext->device, &allocInfo,
                               descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }

  for (size_t i = 0; i < swapChainSize; i++) {
    eastl::vector<VkWriteDescriptorSet> descriptorWrites;
    eastl::vector<VkDescriptorImageInfo> imageInfos;

    if (!buffers.empty()) {
      VkDescriptorBufferInfo bufferInfo = {};
      bufferInfo.buffer = buffers[i];
      bufferInfo.offset = 0;
      bufferInfo.range = bufferBlockSize;

      VkWriteDescriptorSet desWriteBuffer = {};
      desWriteBuffer.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      desWriteBuffer.dstSet = descriptorSets[i];
      desWriteBuffer.dstBinding = 0;
      desWriteBuffer.dstArrayElement = 0;
      desWriteBuffer.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      desWriteBuffer.descriptorCount = 1;
      desWriteBuffer.pBufferInfo = &bufferInfo;

      descriptorWrites.push_back(desWriteBuffer);

    } else if (!images.empty()) {
      imageInfos.resize(images.size());
      for (auto j = 0; j < images.size(); j++) {
        auto tex = images[j];
        imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[j].imageView = tex.textureImageView;
        imageInfos[j].sampler = tex.textureSampler;

        VkWriteDescriptorSet desWriteImg = {};

        desWriteImg.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        desWriteImg.dstSet = descriptorSets[i];
        desWriteImg.dstBinding = j;
        desWriteImg.dstArrayElement = 0;
        desWriteImg.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        desWriteImg.descriptorCount = 1;
        desWriteImg.pImageInfo = &imageInfos[j];

        descriptorWrites.push_back(desWriteImg);
      }
    }

    vkUpdateDescriptorSets(vkContext->device,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);
  }

  return FrameResource<VkDescriptorSet>(descriptorSets);
}

// TODO this part should is really dynamic
RID GpuResourceManager::createIndexedDrawCommandBuffers(
    VkWindowContext windowContext,
    RID meshObjId,
    RID descriptorSets,
    VkRenderPass renderPass,
    VkPipeline graphicsPipeline,
    VkPipelineLayout pipelineLayout,
    std::vector<VkFramebuffer> swapChainFramebuffers,
    VkClearColorValue clearColor) {
  eastl::vector<VkCommandBuffer> commandBuffers(
      windowContext.swapChainImages.size());

  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = vkContext->commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  if (vkAllocateCommandBuffers(vkContext->device, &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  auto mesh = *(getById<crevice::SharedPtr<VkMesh>>(meshObjId));

  for (size_t i = 0; i < commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                   // Optional
    beginInfo.pInheritanceInfo = nullptr;  // Optional

    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[i];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = windowContext.swapChainExtent;

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline);
    VkBuffer vertexBuffers[] = {mesh.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers[i], mesh.indexBuffer, 0,
                         VK_INDEX_TYPE_UINT32);

    auto desSet = getById<DescriptorSets>(descriptorSets);
    vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, &desSet[i], 0, nullptr);
    vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(mesh.indicesSize),
                     1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffers[i]);
    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  auto rid = rand();
  this->commandBuffers[rid] = commandBuffers;
  return rid;
}

// uint8_t GpuResourceManager::swapChainSize = 1;