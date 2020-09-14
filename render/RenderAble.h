#ifndef MYVKLEARN_RENDERABLE_H
#define MYVKLEARN_RENDERABLE_H

#include <vector>

#include "common/Resource.h"

class RenderAble {
  RID mesh;
  // TODO conbine to materialInstance
  std::vector<RID> textures;
  RID shaderPack;
};

#endif MYVKLEARN_RENDERABLE_H