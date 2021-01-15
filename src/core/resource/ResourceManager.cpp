#include "resource/ResourceManager.h"

crevice::SharedPtr<Mesh> ResourceManager::addMesh(RID rid, Mesh mesh) {
  std::unique_lock lock(meshMutex);
  auto m = eastl::make_shared<Mesh>(mesh);
  meshes.emplace(rid, m);
  return m;
}

crevice::SharedPtr<crevice::Image2D> ResourceManager::addTexture2D(RID rid, crevice::Image2D tex) {
  std::unique_lock lock(texMutex);
  auto t = eastl::make_shared<crevice::Image2D>(tex);
  textures.emplace(rid, t);
  return t;
}
