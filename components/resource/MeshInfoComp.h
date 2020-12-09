/**
 * @file MeshInfoComp.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "common/Resource.h"

/**
 * @brief Mesh information component
 * 
 */
struct MeshInfoComp {
  //TODO multiple mesh
  RID meshId;
  bool autoLoadMem = true;
  bool autoLoadGpu = true;
};
