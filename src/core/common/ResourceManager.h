#pragma once

#include <shared_mutex>

#include "common/Resource.h"
#include "resource/Model.h"
#include "resource/Image.h"
#include "stl/CreviceHashMap.h"
#include "stl/CreviceSharedPtr.h"



class ResourceManager {
 private:
   crevice::HashMap<RID,crevice::SharedPtr<Mesh>> meshes;
   mutable std::shared_mutex meshMutex;

   crevice::HashMap<RID,crevice::SharedPtr<crevice::Image2D>> textures;
   mutable std::shared_mutex texMutex;
   // std::map<RID, myvk::ShaderPack> shaders;


 public:
  // RID loadFromObj(std::string modelPath);
  crevice::SharedPtr<Mesh> addMesh(RID, Mesh);
  crevice::SharedPtr<crevice::Image2D> addTexture2D(RID, crevice::Image2D);

  // Mesh createMeshFromObjPath(std::string modelPath);

  
  /**
   * @brief garbage collection
   * TODO
   */ 
  void gc();

  template <typename T>
  T getById(RID rid){};

   template <typename T>
  crevice::SharedPtr<T> getOrNullById(RID rid){};

  template <>
  crevice::SharedPtr<Mesh> getOrNullById<Mesh>(RID rid) {
    if (meshes.count(rid) == 0)
    {
      return nullptr;
    }
    return meshes.find(rid)->second;
  }

  template <>
  crevice::SharedPtr<crevice::Image2D> getOrNullById<crevice::Image2D>(RID rid) {
    std::shared_lock lock(texMutex);
    if (textures.count(rid) == 0)
    {
      return nullptr;
    }
    return textures.find(rid)->second;
  }

  template <>
  crevice::SharedPtr<Mesh> getById<crevice::SharedPtr<Mesh>>(RID rid) {
    std::shared_lock lock(meshMutex);
    return meshes.find(rid)->second;
  }

  template <>
  crevice::SharedPtr<crevice::Image2D> getById<crevice::SharedPtr<crevice::Image2D>>(RID rid) {
    std::shared_lock lock(meshMutex);
    return textures.find(rid)->second;
  }

  // Mesh getMesh(RID rid) { return meshes.find(rid)->second; }
};