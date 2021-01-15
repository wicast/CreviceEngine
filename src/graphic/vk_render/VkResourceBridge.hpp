#pragma once

#include "resource/ResourceManager.h"
#include "GpuResourceManager.h"
#include "RenderServer.h"

namespace crevice {

/**
 * @brief The bridge between Vulkan and Mem Resource
 * This class is used for loading resource from Memory to gpu
 */
class VkResourceBridge {
 private:
  /* data */
  static VkResourceBridge* instance;
  ResourceManager* mResManger;
  GpuResourceManager* mGpuResManger;
  VkContext* mVkContext;

  void loadMeshFromMem(RID rid, VkMesh& vkMesh);

  void createVkTextureFromMem(RID rid, VkTexture& vkTex);

 public:
  static VkResourceBridge* getInstance() {
    if (instance == nullptr) {
      instance = new VkResourceBridge();
    }
    return instance;
  }

  void setup(ResourceManager* resManger,
             GpuResourceManager* gResManger,
             VkContext* vkContext);

  crevice::SharedPtr<VkMesh> createVkMeshFromVertices(RID rid);

  crevice::SharedPtr<VkTexture> createVkTextureFromMemTex(RID rid);

  VkResourceBridge(/* args */);
  ~VkResourceBridge();
};

}  // namespace crevice
