#pragma once

#include <cstdint>
#include "stl/CreviceVector.h"

namespace crevice {

enum ShaderSlotType{
    Bool,Int,Uint,Float,Double,
    Bvec1,Bvec2,Bvec3,Bvec4,
    Ivec1,Ivec2,Ivec3,Ivec4,
    Uvec1,Uvec2,Uvec3,Uvec4,
    Vec1,Vec2,Vec3,Vec4,
    Mat2x2,mat3x2,Mat4x2,Mat2x3,Mat3x3,Mat4x3,Mat2x4,Mat3x4,Mat4x4,
    TextureSample2D,
    PixelLocalTextureSampler
};

//shaderInputKey generate descriptorSetLayout
//shaderInputKey generate shader input uniforms
struct ShaderInputKey {
  //
  crevice::Vector<crevice::ShaderSlotType> keys;

   bool operator==(const ShaderInputKey& other) const {
      return keys == other.keys;
  }
};

}  // namespace crevice