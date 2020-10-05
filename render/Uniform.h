#ifndef MYVK_GPU_UNIFORM_H
#define MYVK_GPU_UNIFORM_H 1

#include <common\GLMath.h>

struct UniformBufferObject {
  glm::mat4 model;
  glm::mat4 view;
  glm::mat4 proj;

  alignas(16) glm::vec3 viewPos;

  alignas(16) glm::vec3 lightPosition;
  alignas(16) glm::vec3 lightDiffuse;
};

#endif