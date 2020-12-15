#include "VkResourceBridge.hpp"

namespace crevice {

void VkResourceBridge::setup(ResourceManager* resManger,
                             GpuResourceManager* gResManger,
                             VkContext* vkContext) {
  mResManger = resManger;
  mVkContext = vkContext;
  mGpuResManger = gResManger;
}

void VkResourceBridge::loadMeshFromMem(RID rid, VkMesh& vkMesh) {
  auto mesh = *(mResManger->getById<crevice::SharedPtr<Mesh>>(rid));

  //   VkMesh vkMesh{};

  vkMesh.indicesSize = mesh.indices.size();
  vkMesh.vertexSize = mesh.vertices.size();
  VkDeviceSize bufferSize = sizeof(Vertex) * vkMesh.vertexSize;
  vkMesh.bufferSize = bufferSize;

  VkBuffer stagingBuffer = VK_NULL_HANDLE;
  VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;

  mGpuResManger->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer, stagingBufferMemory);

  void* data;
  vkMapMemory(mVkContext->device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, mesh.vertices.data(), (size_t)bufferSize);
  vkUnmapMemory(mVkContext->device, stagingBufferMemory);

  mGpuResManger->createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkMesh.vertexBuffer,
      vkMesh.vertexBufferMemory);

  mGpuResManger->copyBuffer(stagingBuffer, vkMesh.vertexBuffer, bufferSize);

  vkDestroyBuffer(mVkContext->device, stagingBuffer, nullptr);
  vkFreeMemory(mVkContext->device, stagingBufferMemory, nullptr);

  // IndexBuffer
  bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();

  mGpuResManger->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer, stagingBufferMemory);

  vkMapMemory(mVkContext->device, stagingBufferMemory, 0, bufferSize, 0, &data);
  memcpy(data, mesh.indices.data(), (size_t)bufferSize);
  vkUnmapMemory(mVkContext->device, stagingBufferMemory);

  mGpuResManger->createBuffer(
      bufferSize,
      VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vkMesh.indexBuffer,
      vkMesh.indexBufferMemory);

  mGpuResManger->copyBuffer(stagingBuffer, vkMesh.indexBuffer, bufferSize);

  vkDestroyBuffer(mVkContext->device, stagingBuffer, nullptr);
  vkFreeMemory(mVkContext->device, stagingBufferMemory, nullptr);

  //   return vkMesh;
}

void VkResourceBridge::createVkTextureFromMem(RID rid, VkTexture& newTex) {
  auto tex = *(mResManger->getById<crevice::SharedPtr<crevice::Image2D>>(rid));
  auto pixels = tex.rawL0.rawData;
  auto imageSize = tex.rawL0.size;
  auto texWidth = tex.width;
  auto texHeight = tex.height;
  auto mipLevels = tex.mipLevels;

  newTex.width = texWidth;
  newTex.height = texHeight;
  newTex.mipLevels = mipLevels;

//   crevice::VkTexture newTex{};

  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;
  mGpuResManger->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                              stagingBuffer, stagingBufferMemory);
  void* data;
  vkMapMemory(mVkContext->device, stagingBufferMemory, 0, imageSize, 0, &data);
  memcpy(data, pixels, static_cast<size_t>(imageSize));
  vkUnmapMemory(mVkContext->device, stagingBufferMemory);

  mGpuResManger->createImage(texWidth, texHeight, mipLevels,
                             VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
                             VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                 VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                 VK_IMAGE_USAGE_SAMPLED_BIT,
                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             newTex.textureImage, newTex.textureImageMemory);

  mGpuResManger->transitionImageLayout(
      newTex.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
  mGpuResManger->copyBufferToImage(stagingBuffer, newTex.textureImage,
                                   static_cast<uint32_t>(texWidth),
                                   static_cast<uint32_t>(texHeight));
  mGpuResManger->generateMipmaps(newTex.textureImage, VK_FORMAT_R8G8B8A8_UNORM,
                                 texWidth, texHeight, mipLevels);

  vkDestroyBuffer(mVkContext->device, stagingBuffer, nullptr);
  vkFreeMemory(mVkContext->device, stagingBufferMemory, nullptr);

  newTex.textureImageView = mGpuResManger->createImageView(
      newTex.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT,
      mipLevels);

  // TODO some image like depth don't need sampler sometime
  mGpuResManger->createTextureSampler(mipLevels, newTex.textureSampler);
}

crevice::SharedPtr<VkMesh> VkResourceBridge::createVkMeshFromVertices(RID rid) {
  auto m = mGpuResManger->getOrNew<VkMesh>(rid);
  loadMeshFromMem(rid, *m);
  return m;
}

crevice::SharedPtr<VkTexture> VkResourceBridge::createVkTextureFromMemTex(
    RID rid) {
    auto vTex = mGpuResManger->getOrNew<VkTexture>(rid);
    createVkTextureFromMem(rid, *vTex);
    return vTex;
}

VkResourceBridge::VkResourceBridge(/* args */) {}

VkResourceBridge::~VkResourceBridge() {}

VkResourceBridge* VkResourceBridge::instance = nullptr;

}  // namespace crevice
