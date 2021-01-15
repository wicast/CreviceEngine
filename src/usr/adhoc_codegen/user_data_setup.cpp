#include "../user_data_setup.hpp"

#include "components/Camera.h"
#include "components/Transform.h"
#include "components/resource/MaterialInfoComp.h"
#include "components/resource/MeshInfoComp.h"
#include "components/resource/VkMeshRes.hpp"
#include "components/resource/VkTextureResComp.hpp"

void usr_ecs_scene_setup(flecs::world& ecs) {
  auto cam = ecs.entity();
  cam.set<Camera>(Camera());

  auto obj = ecs.entity();
  Transform t{};
  t.position = {0.0f, 0.0f, -5.0f};
  obj.set<Transform>(t);

  // TODO get rid from asset
  // obj.set<MeshInfoComp>(MeshInfoComp{5, true, true});
  obj.set<MeshInfoComp>(MeshInfoComp{0, true, true});
  obj.set<MaterialInfoComp>(MaterialInfoComp{3, 4, true, true, false});
}