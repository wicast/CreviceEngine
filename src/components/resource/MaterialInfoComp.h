/**
 * @file MaterialInfoComp.h
 * @author wicast (wicast@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include <vector>

#include "3rd/volk/volk_imp.h"
#include "common/Resource.h"
#include "graphic/vk_render/ShaderPack.h"

/**
 * @brief Material Handle
 *
 */
struct MaterialInfoComp {
  // TODO dynamic textures
  RID obj1TexId, specTexId;
  bool autoLoadTexMem = true;
  bool autoLoadTexGpu = true;
  bool loaded = false;
};
