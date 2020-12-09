#pragma once

#include "flecs.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "components/resource/MeshInfoComp.h"
#include "components/resource/MaterialInfoComp.h"


namespace crevice {
class SceneManager {
 private:
  /* data */
 public:
  static void setUpScene(flecs::world &ecs) {
    auto cam = ecs.entity();
    cam.set<Camera>(Camera());

    auto obj = ecs.entity();
    Transform t;
    t.position = {0.0f, 0.0f, -5.0f};
    obj.set<Transform>(t);

    // TODO get RID from asset
    obj.set<MeshInfoComp>(MeshInfoComp{});
    obj.set<MaterialInfoComp>(MaterialInfoComp{});
  }

  SceneManager(/* args */);
  ~SceneManager();
};

SceneManager::SceneManager(/* args */) {}

SceneManager::~SceneManager() {}

}  // namespace crevice
