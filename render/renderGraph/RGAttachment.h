#ifndef CREVICE_RG_ATTACHMENT
#define CREVICE_RG_ATTACHMENT 1

#include <cstdint>
#include "render/Texture.h"
#include "stl/CreviceString.h"

namespace crevice{

enum RGAttachmentTypes {
    Color,DepthStencil,Present
};

struct RGAttachment
{
    /* data */
    //TODO types
    RGAttachmentTypes type;
    uint32_t id;
    String name;
    VkFormat format;

    VkAttachmentLoadOp loadOp;
    VkAttachmentStoreOp storeOp;
    VkAttachmentLoadOp stencilLoadOp;
    VkAttachmentStoreOp stencilStoreOp;

    //if null, then this is temp texture for inner render
    bool externalTexture;
};

}

#endif