#pragma once

#include "flecs.h"

#include "common/CVTimer.h"
#include "common/CamControl.h"
#include "components/Camera.h"
#include "components/Transform.h"
#include "graphic/vk_render/Uniform.h"

#include "components/resource/MaterialInfoComp.h"
#include "components/resource/MeshInfoComp.h"

#include "components/resource/VkMeshRes.hpp"

#include "components/RenderHandler.h"

#include "volk_imp.h"

#include "common/InputState.h"

void updateCamera(flecs::entity e, Camera& cam) {
  using namespace crevice;
  // TODO add command mapping
  if (InputState::getKey(CV_KEY_W))
    cam.ProcessKeyboard(FORWARD, CVTimer::delta());
  if (InputState::getKey(CV_KEY_S))
    cam.ProcessKeyboard(BACKWARD, CVTimer::delta());
  if (InputState::getKey(CV_KEY_A))
    cam.ProcessKeyboard(LEFT, CVTimer::delta());
  if (InputState::getKey(CV_KEY_D))
    cam.ProcessKeyboard(RIGHT, CVTimer::delta());
  if (InputState::getKey(CV_KEY_E))
    cam.ProcessKeyboard(UP, CVTimer::delta());
  if (InputState::getKey(CV_KEY_Q))
    cam.ProcessKeyboard(DOWN, CVTimer::delta());

  CamControl::updateXY(InputState::xpos, InputState::ypos);
  cam.ProcessMouseMovement(CamControl::xoffset, CamControl::yoffset);
}

void updateTransform(flecs::entity e, Transform& t) {
  t.rotation = {0.f, crevice::CVTimer::duration() * 100.0f, 0.0f};
}

// This is not perframe
// Query with $RenderHandler
// void loadPerObjRenderAbleAsset(flecs::iter it,
//                                Transform objs[],
//                                MeshInfoComp meshes[],
//                                MaterialInfoComp mats[]) {
//   using namespace crevice;
//   auto renderServer = RenderServer::getInstance();
//   auto renderHandler = it.column<RenderHandler>(4);

//   // TODO this should load by mesh handler string
  
//   // auto obj1 = renderServer->gpuRManager->addModel(MODEL_PATH_tmp);
//   // renderServer->gpuRManager->generateVkMeshBuffer(obj1);

//   // auto obj2 = renderServer->gpuRManager->addModel(APPLE_MODEL_PATH_tmp);
//   // renderServer->gpuRManager->generateVkMeshBuffer(obj2);

//   // auto obj1TexId = renderServer->gpuRManager->createMyTexture(TEXTURE_PATH_tmp);
//   // auto specTexId =
//   //     renderServer->gpuRManager->createMyTexture(SPEC_TEXTURE_PATH_tmp);

//   // for (auto i : it) {
//   //   auto& mesh = meshes[i];
//   //   mesh.meshId = obj1;

//   //   auto& mat = mats[i];
//   //   mat.obj1TexId = obj1TexId;
//   //   mat.specTexId = specTexId;
//   // }
// }


void setCustSys(flecs::world& ecs) {
    ecs.system<Camera>().each(updateCamera);

  ecs.system<Transform>().each(updateTransform);
}