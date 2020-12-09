#pragma once

#include "common/Resource.h"
#include "render/resource/VkMesh.hpp"
#include "stl/CreviceSharedPtr.h"

struct VkMeshRes {
  RID rid;
  crevice::SharedPtr<VkMesh> mesh;
};
