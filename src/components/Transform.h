/**
 * @file Transform.h
 * @author wicast (wicast@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2020-11-25
 *
 * @copyright Copyright (c) 2020
 *
 */

#pragma once

#include "common/GLMath.h"
#include "graphic/vk_render/descriptor/shaderInputKey.h"

/**
 * @brief Transform component
 *
 */
class Transform {
  /* data */
  glm::mat4 model = glm::mat4(1.f);

  /**
   * @brief 
   * 
   */
  void calModelMatrix() {
    model = glm::translate(glm::mat4(1.f), position);
    auto q = glm::quat(glm::radians(rotation));
    model *= glm::mat4_cast(q);
    model = glm::scale(model, scale);
  }

 public:
  glm::vec3 position = {0.f, 0.f, 0.f};
  glm::vec3 rotation = {0.f, 0.f, 0.f};
  glm::vec3 scale = {1.f, 1.f, 1.f};

  /**
   * @brief Get the Model Matrix object
   *
   * @return glm::mat4
   */
  glm::mat4 getModelMatrix() {
    calModelMatrix();
    return model;
  };

  // const crevice::ShaderInputKey shaderInputKey =
  // {{crevice::ShaderSlotType::Mat4x4}};
};
