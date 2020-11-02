#pragma once

#include "common/GLMath.h"
#include "render/descriptor/shaderInputKey.h"

class Transform
{
  /* data */
  glm::mat4 model = glm::mat4(1.f);

public:
  glm::vec3 position = {0.f, 0.f, 0.f};
  glm::vec3 rotation = {0.f, 0.f, 0.f};
  glm::vec3 scale = {1.f, 1.f, 1.f};

  glm::mat4 getModelMatrix()
  {
    calModelMatrix();
    return model;
  };

  void calModelMatrix()
  {
    model = glm::translate(glm::mat4(1.f), position);
    auto q = glm::quat(glm::radians(rotation));
    model *= glm::mat4_cast(q);
    model = glm::scale(model, scale);
  }

  // const crevice::ShaderInputKey shaderInputKey =
  // {{crevice::ShaderSlotType::Mat4x4}};
};
