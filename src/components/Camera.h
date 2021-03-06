/**
 * @file Camera.h
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
 * @brief Defines several possible options for camera movement. Used as abstraction to \n
 *  stay away from window-system specific input methods
 * 
 */
enum Camera_Movement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN,
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

/**
 * @brief 
 * 
 */
class Camera {
 private:
  /* data */
 public:
  // Camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // Euler Angles
  float Yaw;
  float Pitch;
  // Camera options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  // const crevice::ShaderInputKey shaderInputKey =
  // {{crevice::ShaderSlotType::mat4x4, crevice::ShaderSlotType::mat4x4}};

  Camera();
  Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
  Camera(float posX,
         float posY,
         float posZ,
         float upX,
         float upY,
         float upZ,
         float yaw,
         float pitch);
  glm::mat4 GetViewMatrix();
  glm::mat4 GetProjMatrix();
  void ProcessMouseMovement(float xoffset,
                            float yoffset,
                            bool constrainPitch = true);
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);
  void ProcessMouseScroll(float yoffset);
  void updateCameraVectors();
  ~Camera();
};
