/**
 * @file RGAttachment.h
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

#include "resource/Image.h"
#include "stl/CreviceString.h"

namespace crevice {

/**
 * @brief RGAttachment Types
 * 
 */
enum RGAttachmentTypes { Color, DepthStencil, Present };

/**
 * @brief RGAttachment
 * 
 */
struct RGAttachment {
  /* data */
  RGAttachmentTypes type;
  uint32_t id;
  String name;
  VkFormat format;

  VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

  // if null, then this is temp texture for inner render
  bool externalTexture;
};

}  // namespace crevice
