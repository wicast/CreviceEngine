#ifndef CREVICE_COMPONENT_TRANSFORM_H
#define CREVICE_COMPONENT_TRANSFORM_H 1

#include <common\GLMath.h>
#include "render/descriptor/shaderInputKey.h"

class Transform
{
    /* data */
public:
    glm::mat4 model;
    
    // const crevice::ShaderInputKey shaderInputKey = {{crevice::ShaderSlotType::Mat4x4}};
    
};

#endif