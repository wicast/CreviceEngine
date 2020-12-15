#pragma once

#include "flecs.h"

#include "components/Camera.h"
#include "components/RenderHandler.h"
#include "components/Transform.h"
#include "components/resource/MeshInfoComp.h"
#include "components/resource/MeshLoadingSt.hpp"
#include "components/resource/MeshRes.hpp"
#include "components/resource/VkMeshLoadingSt.hpp"

#include "components/resource/MaterialInfoComp.h"
#include "components/resource/ImageResComp.hpp"
#include "components/resource/VkTextureResComp.hpp"


void ecs_reg_comp(flecs::world& ecs) {
    ecs.component<Camera>();
    ecs.component<RenderHandler>();
    ecs.component<Transform>();
    ecs.component<MeshInfoComp>();
    ecs.component<MeshLoadingSt>();
    ecs.component<MeshRes>();
    ecs.component<VkMeshLoadingSt>();
    ecs.component<MaterialInfoComp>();
    ecs.component<ImageResComp>();
    ecs.component<VkTextureResComp>();
}