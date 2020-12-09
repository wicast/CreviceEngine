#pragma once

#include "common/AssetResourceBridge.hpp"
#include "common/ResourceManager.h"
#include "components/resource/MeshInfoComp.h"

#include "components/resource/ImageResComp.hpp"
#include "components/resource/MaterialInfoComp.h"
#include "components/resource/MeshLoadingSt.hpp"
#include "components/resource/MeshRes.hpp"
#include "components/resource/VkMeshLoadingSt.hpp"
#include "components/resource/VkMeshRes.hpp"
#include "components/resource/VkTextureResComp.hpp"

#include "render/VkResourceBridge.hpp"
#include "render/resource/VkMesh.hpp"

#include <future>
#include "flecs.h"

template <typename R>
bool is_ready(std::shared_future<R> const& f) {
  return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

// Mesh loading systems

/**
 * @brief do OnSet
 *
 * @param e
 * @param meshInfoComp
 */
void onSetMesh(flecs::entity e, MeshInfoComp meshInfoComp) {
  if (meshInfoComp.autoLoadMem) {
    MeshLoadingSt st{};
    st.rid = meshInfoComp.meshId;
    e.set<MeshLoadingSt>(st);
  }
}

/**
 * @brief do with $AssetResourceBridge
 *
 * @param e
 * @param meshLoadingSt
 */
void loadMesh(flecs::entity e, MeshLoadingSt& meshLoadingSt) {
  if (!meshLoadingSt.loading) {
    auto bridge = AssetResourceBridge::getInstance();

    meshLoadingSt.meshF =
        std::async(std::launch::async, &AssetResourceBridge::loadMesh, bridge,
                   meshLoadingSt.rid)
            .share();
    meshLoadingSt.loading = true;
  } else if (meshLoadingSt.meshF.valid() && is_ready(meshLoadingSt.meshF)) {
    e.set<MeshRes>({meshLoadingSt.rid, meshLoadingSt.meshF.get()});
    e.remove<MeshLoadingSt>();
    return;
  }
}

/**
 * @brief set to load VkMesh when a mesh loaded
 *
 * @param e
 * @param meshInfoComp
 * @param meshRes
 */
void onSetMeshRes(flecs::entity e, MeshInfoComp meshInfoComp, MeshRes meshRes) {
  if (meshInfoComp.autoLoadGpu) {
    VkMeshLoadingSt st{};
    st.rid = meshInfoComp.meshId;
    e.set<VkMeshLoadingSt>(st);
  }
}

/**
 * @brief do VkMesh Loading
 * Filter with VkMeshLoadingSt
 *
 * @param e
 * @param st
 * @param meshRes
 */
void loadVkMesh(flecs::entity e, VkMeshLoadingSt& st, MeshRes meshRes) {
  if (!st.loading) {
    auto bridge = crevice::VkResourceBridge::getInstance();

    st.vkMeshF =
        std::async(std::launch::async,
                   &crevice::VkResourceBridge::createVkMeshFromVertices, bridge,
                   st.rid)
            .share();
    st.loading = true;
  } else if (st.vkMeshF.valid() && is_ready(st.vkMeshF)) {
    e.set<VkMeshRes>({st.rid, st.vkMeshF.get()});
    e.remove<VkMeshLoadingSt>();
    return;
  }
}

// Texture loading systems

void onSetMaterial(flecs::entity& e, MaterialInfoComp matInfo) {
  if (!matInfo.autoLoadTexMem) {
    return;
  }
  ImageResComp st{};
  // TODO iter textures
  st.unloaded.push_back(matInfo.obj1TexId);
  st.unloaded.push_back(matInfo.specTexId);
  e.set<ImageResComp>(st);
}

void loadImageFromAsset(flecs::entity e, ImageResComp& imageRes) {
  // Start async texture loading
  for (auto ridUnloaded : imageRes.unloaded) {
    auto bridge = AssetResourceBridge::getInstance();
    imageRes.imagesLoading.emplace(
        ridUnloaded,
        std::async(std::launch::async, &AssetResourceBridge::loadImage, bridge,
                   ridUnloaded));
  }
  imageRes.unloaded.clear();

  // Check loaded
  for (auto loading : imageRes.imagesLoading) {
    if (loading.second.valid() && is_ready(loading.second)) {
      imageRes.imagesLoaded.emplace(loading.first, loading.second.get());
      imageRes.imagesLoadedContinue.push_back(loading.first);
      imageRes.imagesLoading.erase(loading.first);
    }
  }
}

void triggerLoadVkTex(flecs::entity e, ImageResComp& imageRes) {
  // HACK this will replace by data broadcast
  if (imageRes.imagesLoadedContinue.empty()) {
    return;
  }
  auto st = e.get_mut<VkTextureResComp>();
  st->unloaded.insert(st->unloaded.begin(),
                      imageRes.imagesLoadedContinue.begin(),
                      imageRes.imagesLoadedContinue.end());

  imageRes.imagesLoadedContinue.clear();
}

void loadVkTextureFromMem(flecs::entity e,
                          MaterialInfoComp& matInfo,
                          ImageResComp& imageRes,
                          VkTextureResComp& vkTextureRes) {
  if (!matInfo.autoLoadTexGpu) {
    return;
  }
  if (!vkTextureRes.unloaded.empty()) {
    for (auto imageLoaded : vkTextureRes.unloaded) {
      auto bridge = crevice::VkResourceBridge::getInstance();
      vkTextureRes.textureLoading.emplace(
          imageLoaded,
          std::async(std::launch::async,
                     &crevice::VkResourceBridge::createVkTextureFromMemTex,
                     bridge, imageLoaded));
    }
    vkTextureRes.unloaded.clear();
  }
  if (!vkTextureRes.textureLoading.empty()) {
    for (auto loading : vkTextureRes.textureLoading) {
      if (loading.second.valid() && is_ready(loading.second)) {
        vkTextureRes.textureLoaded.emplace(loading.first,
                                                 loading.second.get());
        vkTextureRes.textureLoading.erase(loading.first);
      }
    }
    if (imageRes.imagesLoading.empty() &&
        vkTextureRes.textureLoading.empty() && !matInfo.loaded) {
      matInfo.loaded = true;
      // HACK Just for trigger OnSet
      e.set<VkTextureResComp>(vkTextureRes);
    }
  }
}

void setAssetResourceSystems(flecs::world& ecs) {
  // load mesh to mem
  ecs.system<MeshInfoComp>().kind(flecs::OnSet).each(onSetMesh);
  ecs.system<MeshLoadingSt>().each(loadMesh);

  // load VkMesh from mem to gpu
  ecs.system<MeshInfoComp, MeshRes>().kind(flecs::OnSet).each(onSetMeshRes);
  ecs.system<VkMeshLoadingSt, MeshRes>().each(loadVkMesh);

  // load texture
  ecs.system<MaterialInfoComp>().kind(flecs::OnSet).each(onSetMaterial);
  ecs.system<ImageResComp>().each(loadImageFromAsset);
  ecs.system<ImageResComp>().kind(flecs::OnUpdate).each(triggerLoadVkTex);
  ecs.system<MaterialInfoComp, ImageResComp, VkTextureResComp>().each(
      loadVkTextureFromMem);
}