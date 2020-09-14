#ifndef CREVICE_RG_ATTACHMENT
#define CREVICE_RG_ATTACHMENT 1

#include <cstdint>
#include "render/Texture.h"
#include "stl/CreviceString.h"

namespace crevice{

struct RGAttachment
{
    /* data */
    //TODO types
    uint32_t type;
    uint32_t id;
    String name;

    //if null, then this is temp texture for inner render
    CVTexture textureRef;
    bool externalTexture;
};

}

#endif