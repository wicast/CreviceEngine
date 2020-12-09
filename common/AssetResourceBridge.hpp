#pragma once
#include "assetsManager/AssetManager.hpp"
#include "common/Resource.h"
#include "common/ResourceManager.h"
#include "eastl/string.h"

/**
 * @brief Bridge between Asset System and Mem Resource
 *
 */
class AssetResourceBridge {
 private:
  static AssetResourceBridge* instance;

  ResourceManager* rManager;
  AssetManager* aManager;

  Mesh createMeshFromObjPath(eastl::string modelPath);

  crevice::Image2D createTex2dFromPath(eastl::string path);

 public:
  static AssetResourceBridge* getInstance() {
    if (instance == nullptr) {
      instance = new AssetResourceBridge();
    }
    return instance;
  }

  void setup(ResourceManager* rm, AssetManager* am);

  crevice::SharedPtr<Mesh> loadMesh(RID rid) {
    auto path = aManager->getPathByRID(rid);
    return rManager->addMesh(rid, createMeshFromObjPath(path));
  }

  crevice::SharedPtr<crevice::Image2D> loadImage(RID rid) {
    auto img = rManager->getOrNullById<crevice::Image2D>(rid);
    if (img != nullptr) {
      return img;
    }

    // TODO deal with archive pipeline, etc.
    auto path = aManager->getPathByRID(rid);
    return rManager->addTexture2D(rid, createTex2dFromPath(path));
  }

  AssetResourceBridge(/* args */);
  ~AssetResourceBridge();
};
