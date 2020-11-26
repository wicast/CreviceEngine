/**
 * @file VertexInputInfo.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include <cstdint>
#include "3rd/volk/volk_imp.h"
#include "stl/CreviceVector.h"

namespace crevice {
/**
 * @brief Vertex Binding Description
 * 
 */
struct BindingDescription {
  /* data */
  uint32_t stride;
  VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
};

/**
 * @brief Vertex Attribute Description
 * 
 */
struct AttributeDescription {
  VkFormat format;
  uint32_t offset;

};

/**
 * @brief Vertex InputInfo
 * 
 */
struct VertexInputInfo {
  BindingDescription bindingDescription;
  Vector<AttributeDescription> attributeDescriptions;
};

}  // namespace crevice
