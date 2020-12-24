#include "renderGraph/RenderPass.h"

namespace crevice {

std::tuple<VkVertexInputBindingDescription,
           Vector<VkVertexInputAttributeDescription>>
RenderPass::generateVertexInputInfo() {
  VkVertexInputBindingDescription bindingDescription = {};
  bindingDescription.binding = 0;
  bindingDescription.stride = vertexInputInfoDesc.bindingDescription.stride;
  bindingDescription.inputRate =
      vertexInputInfoDesc.bindingDescription.inputRate;

  Vector<VkVertexInputAttributeDescription> attributeDescriptions = {};

  uint8_t locationCount = 0;
  for (auto attr : vertexInputInfoDesc.attributeDescriptions) {
    VkVertexInputAttributeDescription attributeDescription;
    attributeDescription.binding = 0;
    attributeDescription.location = locationCount;
    attributeDescription.format = attr.format;
    attributeDescription.offset = attr.offset;

    attributeDescriptions.push_back(attributeDescription);
    locationCount++;
  }

  return {bindingDescription, attributeDescriptions};
}

void RenderPass::generateDescriptorSetLayout(GpuResourceManager& gManager) {
  // generate Layouts
  Vector<VkDescriptorSetLayoutBinding> bufferBindings;
  Vector<VkDescriptorSetLayoutBinding> perPassTexBindings;
  Vector<VkDescriptorSetLayoutBinding> perObjTexBindings;

  for (auto inKey : perPassInputKeys) {
    Vector<ShaderSlotType> bufferKeys;
    auto inx = 0;
    for (auto key : inKey.keys) {
      if (key == crevice::ShaderSlotType::TextureSample2D) {
        VkDescriptorSetLayoutBinding perTexBinding = {};
        perTexBinding.binding = inx;
        perTexBinding.descriptorCount = 1;
        perTexBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        perTexBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        perPassTexBindings.push_back(perTexBinding);
        inx++;
      } else if (key == crevice::ShaderSlotType::PixelLocalTextureSampler) {
        inx++;
      } else {
        bufferKeys.push_back(key);
      }
    }

    if (!bufferKeys.empty()) {
      VkDescriptorSetLayoutBinding bufferBinding = {};
      bufferBinding.binding = 0;
      bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      // TODO need config or not?
      bufferBinding.descriptorCount = 1;
      // TODO more specific stage flag
      bufferBinding.stageFlags =
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      bufferBinding.pImmutableSamplers = nullptr;  // Optional
      bufferBindings.push_back(bufferBinding);
    }
  }

  for (auto inKey : perObjInputKeys) {
    Vector<ShaderSlotType> bufferKeys;
    auto inx = 0;
    for (auto key : inKey.keys) {
      if (key == crevice::ShaderSlotType::TextureSample2D) {
        VkDescriptorSetLayoutBinding perTexBinding = {};
        perTexBinding.binding = inx;
        perTexBinding.descriptorCount = 1;
        perTexBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        perTexBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        perObjTexBindings.push_back(perTexBinding);
        inx++;
      } else if (key == crevice::ShaderSlotType::PixelLocalTextureSampler) {
        inx++;
      } else {
        bufferKeys.push_back(key);
      }
    }

    if (!bufferKeys.empty()) {
      VkDescriptorSetLayoutBinding bufferBinding = {};
      if (bufferBindings.empty()) {
        bufferBinding.binding = 0;
      } else {
        bufferBinding.binding = 1;
      }
      bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      bufferBinding.descriptorCount = 1;
      bufferBinding.stageFlags =
          VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
      bufferBinding.pImmutableSamplers = nullptr;  // Optional
      bufferBindings.push_back(bufferBinding);
    }
  }

  VkDescriptorSetLayoutCreateInfo bufferLayoutInfo{};
  bufferLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  bufferLayoutInfo.bindingCount = static_cast<uint32_t>(bufferBindings.size());
  bufferLayoutInfo.pBindings = bufferBindings.data();
  // TODO if no buffer
  mSetLayouts.push_back(gManager.addDescriptorSetLayout(bufferLayoutInfo));

  // TODO if no perpassimage
  // VkDescriptorSetLayoutCreateInfo perPassTexLayoutInfo{};
  // perPassTexLayoutInfo.sType =
  //     VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  // perPassTexLayoutInfo.bindingCount =
  //     static_cast<uint32_t>(perPassTexBindings.size());
  // perPassTexLayoutInfo.pBindings = perPassTexBindings.data();
  // mSetLayouts.push_back(gManager.addDescriptorSetLayout(perPassTexLayoutInfo));

  VkDescriptorSetLayoutCreateInfo perObjTexLayoutInfo{};
  perObjTexLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  perObjTexLayoutInfo.bindingCount =
      static_cast<uint32_t>(perObjTexBindings.size());
  perObjTexLayoutInfo.pBindings = perObjTexBindings.data();
  mSetLayouts.push_back(gManager.addDescriptorSetLayout(perObjTexLayoutInfo));
}

void RenderPass::genreatePipeline(GpuResourceManager& gManager,
                                  VkWindowContext* windowContext) {
  auto chainSize = windowContext->swapChainSize;

  auto shaderPack =
      gManager.createShaderPack(std::string(std::get<0>(shaderAsset).data()),
                                std::string(std::get<1>(shaderAsset).data()));

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = shaderPack->vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = shaderPack->fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  auto vertexInfo = generateVertexInputInfo();
  auto bindingDescription = std::get<0>(vertexInfo);
  auto attributeDescriptions = std::get<1>(vertexInfo);

  VkPipelineVertexInputStateCreateInfo vertexInputCInfo = {};
  vertexInputCInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputCInfo.vertexBindingDescriptionCount = 1;
  vertexInputCInfo.pVertexBindingDescriptions =
      &bindingDescription;  // Optional
  vertexInputCInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputCInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)windowContext->swapChainExtent.width;
  viewport.height = (float)windowContext->swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = windowContext->swapChainExtent;

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType =
      VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask =
      VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType =
      VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f;  // Optional
  depthStencil.maxDepthBounds = 1.0f;  // Optional
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front = {};  // Optional
  depthStencil.back = {};   // Optional

  Vector<VkDescriptorSetLayout> _desLayout;
  for (auto RCLayout : mSetLayouts) {
    _desLayout.push_back(*RCLayout);
  }

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(_desLayout.size());
  pipelineLayoutInfo.pSetLayouts = _desLayout.data();

  VkPipelineLayout _pipelineLayout;

  if (vkCreatePipelineLayout(gManager.vkContext->device, &pipelineLayoutInfo,
                             nullptr, &_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  mPipelineLayout = crevice::make_shared<VkPipelineLayout>(_pipelineLayout);

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;

  pipelineInfo.pVertexInputState = &vertexInputCInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pDynamicState = nullptr;
  pipelineInfo.layout = *mPipelineLayout;

  pipelineInfo.renderPass = *mRenderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
  pipelineInfo.basePipelineIndex = -1;               // Optional

  VkPipeline graphicsPipeline{};

  if (vkCreateGraphicsPipelines(gManager.vkContext->device, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  mPipeline = crevice::make_shared<VkPipeline>(graphicsPipeline);

  gManager.destroyShaderPack(*shaderPack);
}

void RenderPass::compile(GpuResourceManager& gManager,
                         //  VectorMap<uint32_t, uint32_t> attachmentMap,
                         SharedPtr<VkRenderPass> vkRenderPass,
                         VkWindowContext* windowContext) {
  // set RenderPass
  mRenderPass = vkRenderPass;

  generateDescriptorSetLayout(gManager);
  // TODO generate shader header

  genreatePipeline(gManager, windowContext);
}

void RenderPass::recordFrameWithSubpass(uint64_t frame,
                                        VkCommandBuffer commandBuffer) {
  // bind pipeline
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, *mPipeline);

  // bind perpass descriptor
  auto perpassDescriptor = mPerpassRenderable.bufferDescriptor;
  vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          *mPipelineLayout, 0, 1,
                          perpassDescriptor.getRes(frame).get(), 0, nullptr);
  // loop renderList
  for (auto renderable : mRenderList) {
    // -- bind meshdata
    auto mesh = renderable.mesh.getRes(frame);
    VkBuffer vertexBuffers[] = {mesh->vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, mesh->indexBuffer, 0,
                         VK_INDEX_TYPE_UINT32);

    // -- bind perobj descriptor
    Vector<VkDescriptorSet> desc;
    if (renderable.bufferDescriptor.prepared) {
      auto bufDesc = renderable.bufferDescriptor.getRes(frame);
      desc.push_back(*bufDesc);
    }
    if (renderable.texDescriptor.prepared) {
      /* code */
      auto texDesc = renderable.texDescriptor.getRes(frame);
      desc.push_back(*texDesc);
    }

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            *mPipelineLayout, 1, desc.size(), desc.data(), 0,
                            nullptr);
    // -- draw
    vkCmdDrawIndexed(commandBuffer, mesh->indicesSize, 1, 0, 0, 0);
  }
}

}  // namespace crevice
