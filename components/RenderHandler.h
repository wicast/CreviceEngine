/**
 * @file RenderHandler.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "render/Uniform.h"
#include "render/renderGraph/RenderGraph.h"

/**
 * @brief Render Handler
 * 
 */
struct RenderHandler {
  crevice::RenderGraph* mainRendergraph;
  uint32_t mainPassId;

  uint8_t currentFrame = 0;
  uint8_t currentImage = 0;

  // TODO splits
  UniformBufferObject ubo;
  crevice::Vector<VkBuffer> cameraUniformBuffers;
  crevice::Vector<VkDeviceMemory> cameraUniformBuffersMemory;
};
