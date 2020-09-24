#pragma once

#include <volk.h>

#include "common/Resource.h"
#include "stl/CreviceSTL.h"
#include "render/FrameResource.h"
#include "render/Model.h"
#include "render/descriptor/ShaderInputKey.h"

namespace crevice {
struct RenderAble {
  uint32_t passId;
  String tags;
 
  ShaderInputKey key;
  FrameResource<VkDescriptorSet> bufferDescriptor;
  FrameResource<VkDescriptorSet> texDescriptor;
  FrameResource<Mesh> mesh;

   //TOOD PerPass or PerObj,this might not needed
  uint8_t type;
};


struct PerPassRenderAble {
  uint32_t passId;
  String tags;
  //PerPass or PerObj
  uint8_t type;
  ShaderInputKey key;
  FrameResource<VkDescriptorSet> descriptor;
  FrameResource<Mesh> mesh;

};
}