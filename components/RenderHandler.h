#pragma once

#include "render/Uniform.h"
#include "render/renderGraph/RenderGraph.h"

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
