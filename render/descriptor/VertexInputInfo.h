#pragma once

#include <cstdint>
#include "3rd/volk/volk_imp.h"
#include "stl/CreviceVector.h"

namespace crevice {
struct BindingDescription {
  /* data */
  uint32_t stride;
  VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
};

struct AttributeDescription {
  VkFormat format;
  uint32_t offset;

};

struct VertexInputInfo {
  BindingDescription bindingDescription;
  Vector<AttributeDescription> attributeDescriptions;
};

}  // namespace crevice
