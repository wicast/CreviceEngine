#pragma once

#include <volk.h>

#include "common/Resource.h"
#include "render/FrameResource.h"
#include "render/resource/Model.h"
#include "render/descriptor/ShaderInputKey.h"
#include "stl/CreviceSTL.h"

namespace crevice {
struct RenderAble {
  uint32_t passId;
  String tags;

  ShaderInputKey key;
  FrameResource<VkDescriptorSet> bufferDescriptor;
  FrameResource<VkDescriptorSet> texDescriptor;
  FrameResource<Mesh> mesh;

  // TOOD PerPass or PerObj,this might not needed
  uint8_t type;
};

struct PerPassRenderAble {
  uint32_t passId;
  String tags;
  // PerPass or PerObj
  uint8_t type;
  ShaderInputKey key;
  FrameResource<VkDescriptorSet> bufferDescriptor;
  // FrameResource<VkDescriptorSet> descriptor;
  FrameResource<Mesh> mesh;
};

struct DescriptorLayoutInfo {
  // buffer=0 or tex=1
  uint8_t type;
  Vector<uint32_t> bindingIds;
};

// Vector<DescriptorLayoutInfo> getBindIngFromKey(ShaderInputKey key) {
//   Vector<DescriptorLayoutInfo> bindInfo(2);
//   for (auto val : key.keys) {
//     if (val == ShaderSlotType::TextureSample2D) {
//       bindInfo[1].type = 1;
//       bindInfo[1].bindingIds.push_back(bindInfo[1].bindingIds.size());
//     } else {
//       if (bindInfo[0].bindingIds.empty()) {
//         bindInfo[0].type = 0;
//         bindInfo[0].bindingIds.push_back(0);
//       }
//     }
//   }
//   return bindInfo;
// }

}  // namespace crevice