#pragma once

#include "common/Resource.h"
#include "eastl/vector_map.h"
#include "stl/CreviceString.h"

// TODO ad-hoc asset mapping
constexpr auto MODEL_PATH_tmp = "../../../../models/monkey.obj";
constexpr auto MODEL2_PATH_tmp = "../../../../models/box2.obj";
constexpr auto APPLE_MODEL_PATH_tmp = "../../../../models/apple.obj";
constexpr auto TEXTURE_PATH_tmp = "../../../../textures/badApple.png";
constexpr auto SPEC_TEXTURE_PATH_tmp =
    "../../../../textures/container2_specular.png";

class AssetManager {
 private:
  /* data */
  // TODO ad-hoc asset mapping
  eastl::vector_map<RID, eastl::string> assetPathMap = {
      {0, MODEL_PATH_tmp},
      {1, MODEL2_PATH_tmp},
      {2, APPLE_MODEL_PATH_tmp},
      {3, TEXTURE_PATH_tmp},
      {4, SPEC_TEXTURE_PATH_tmp}};

 public:
  eastl::string getPathByRID(RID rid) { return assetPathMap[rid]; }

  AssetManager();
  ~AssetManager();
};
