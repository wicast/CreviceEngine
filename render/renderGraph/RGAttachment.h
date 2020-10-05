#ifndef CREVICE_RG_ATTACHMENT
#define CREVICE_RG_ATTACHMENT 1

#include <cstdint>

#include "render/Texture.h"
#include "stl/CreviceString.h"

namespace crevice {

enum RGAttachmentTypes { Color, DepthStencil, Present };

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

#endif