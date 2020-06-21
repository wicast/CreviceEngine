//
// Created by wicas on 2020/1/5.
//

#include "HelloTriangleApplication.h"

// #define STB_IMAGE_IMPLEMENTATION

// #include <stb_image.h>

#include <unordered_map>

const int WIDTH = 800;
const int HEIGHT = 600;

const std::string MODEL_PATH = "../../../../models/box.obj";
const std::string MODEL2_PATH = "../../../../models/box2.obj";
const std::string APPLE_MODEL_PATH = "../../../../models/apple.obj";
const std::string TEXTURE_PATH = "../../../../textures/container2.png";
const std::string SPEC_TEXTURE_PATH =
    "../../../../textures/container2_specular.png";

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct UniformBufferObject {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 proj;

  alignas(16) glm::vec3 viewPos;

  alignas(16) glm::vec3 lightPosition;
  alignas(16) glm::vec3 lightDiffuse;
};

HelloTriangleApplication *HelloTriangleApplication::event_handling_instance;

void HelloTriangleApplication::createResourceManager() {
  resourceManager = ResourceManager{};
}

void HelloTriangleApplication::createGpuResourceManager() {
  gpuResourceManager.initManager(&vkContext);
}

// TODO createSurface with several function
void HelloTriangleApplication::createVkContext() {
  vkContext.enableValidationLayers = enableValidationLayers;

  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  vkContext.createInstance(vkUtil::getRequiredExtensions(
      vkContext.enableValidationLayers, glfwExtensions, glfwExtensionCount));
  createSurface();
  vkContext.pickPhysicalDevice();
  vkContext.createLogicalDevice();

  vkContext.setupDebugMessenger();
}

void HelloTriangleApplication::createSurface() {
  if (glfwCreateWindowSurface(vkContext.instance, windowContext.window, nullptr,
                              &vkContext.surface) != VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

VkShaderModule HelloTriangleApplication::createShaderModule(
    const std::vector<char> &code) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(vkContext.device, &createInfo, nullptr,
                           &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  return shaderModule;
}

void HelloTriangleApplication::createGraphicsPipeline() {
  RID shaderRid = gpuResourceManager.createShaderPack(
      "../../../../shaders/vert.spv", "../../../../shaders/frag.spv");
  myvk::ShaderPack shaderpack = gpuResourceManager.getShaderPack(shaderRid);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = shaderpack.vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = shaderpack.fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo,
                                                    fragShaderStageInfo};

  auto bindingDescription = Vertex::getBindingDescription();
  auto attributeDescriptions = Vertex::getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType =
      VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;  // Optional
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputInfo.pVertexAttributeDescriptions =
      attributeDescriptions.data();  // Optional

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType =
      VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)windowContext.swapChainExtent.width;
  viewport.height = (float)windowContext.swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = windowContext.swapChainExtent;

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

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

  if (vkCreatePipelineLayout(vkContext.device, &pipelineLayoutInfo, nullptr,
                             &pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;

  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pDynamicState = nullptr;
  pipelineInfo.layout = pipelineLayout;

  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;  // Optional
  pipelineInfo.basePipelineIndex = -1;               // Optional

  if (vkCreateGraphicsPipelines(vkContext.device, VK_NULL_HANDLE, 1,
                                &pipelineInfo, nullptr,
                                &graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }

  gpuResourceManager.destoryShaderPack(shaderRid);
}

void HelloTriangleApplication::createRenderPass() {
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = windowContext.swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = findDepthFormat();
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout =
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  std::array<VkAttachmentDescription, 2> attachments = {colorAttachment,
                                                        depthAttachment};
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  ;
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;

  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;
  if (vkCreateRenderPass(vkContext.device, &renderPassInfo, nullptr,
                         &renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed create render pass");
  }
}

void HelloTriangleApplication::createFramebuffers() {
  swapChainFramebuffers.resize(windowContext.swapChainImageViews.size());
  for (size_t i = 0; i < windowContext.swapChainImageViews.size(); i++) {
    std::array<VkImageView, 2> attachments = {
        windowContext.swapChainImageViews[i], depthImageView};
    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = windowContext.swapChainExtent.width;
    framebufferInfo.height = windowContext.swapChainExtent.height;
    framebufferInfo.layers = 1;
    if (vkCreateFramebuffer(vkContext.device, &framebufferInfo, nullptr,
                            &swapChainFramebuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}

// void HelloTriangleApplication::createCommandPool() {
//   QueueFamilyIndices queueFamilyIndices =
//       vkUtil::findQueueFamilies(vkContext.physicalDevice, vkContext.surface);

//   VkCommandPoolCreateInfo poolInfo = {};
//   poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//   poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
//   poolInfo.flags = 0;  // Optional

//   if (vkCreateCommandPool(vkContext.device, &poolInfo, nullptr, &commandPool)
//   !=
//       VK_SUCCESS) {
//     throw std::runtime_error("failed to create command pool!");
//   }
// }

void HelloTriangleApplication::createCommandBuffers() {
  commandBuffers.resize(swapChainFramebuffers.size());
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = vkContext.commandPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

  auto mesh = gpuResourceManager.getMesh(obj1);

  if (vkAllocateCommandBuffers(vkContext.device, &allocInfo,
                               commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }

  for (size_t i = 0; i < commandBuffers.size(); i++) {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                   // Optional
    beginInfo.pInheritanceInfo = nullptr;  // Optional

    if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[i];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = windowContext.swapChainExtent;

    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color = {0.250f, 0.235f, 0.168f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipeline);
    VkBuffer vertexBuffers[] = {mesh.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffers[i], mesh.indexBuffer, 0,
                         VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, &descriptorSets[i], 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffers[i],
                     static_cast<uint32_t>(mesh.indices.size()), 1, 0, 0, 0);

    vkCmdEndRenderPass(commandBuffers[i]);
    if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }
}

void HelloTriangleApplication::createSyncObjects() {
  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  imagesInFlight.resize(windowContext.swapChainImages.size(), VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(vkContext.device, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(vkContext.device, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(vkContext.device, &fenceInfo, nullptr,
                      &inFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores for a frame!");
    }
  }
}

VkFormat HelloTriangleApplication::findSupportedFormat(
    const std::vector<VkFormat> &candidates, VkImageTiling tiling,
    VkFormatFeatureFlags features) {
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(vkContext.physicalDevice, format,
                                        &props);
    if (tiling == VK_IMAGE_TILING_LINEAR &&
        (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
               (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
}

VkFormat HelloTriangleApplication::findDepthFormat() {
  return findSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
       VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool HelloTriangleApplication::hasStencilComponent(VkFormat format) {
  return format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
         format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void HelloTriangleApplication::createBuffer(VkDeviceSize size,
                                            VkBufferUsageFlags usage,
                                            VkMemoryPropertyFlags properties,
                                            VkBuffer &buffer,
                                            VkDeviceMemory &bufferMemory) {
  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(vkContext.device, &bufferInfo, nullptr, &buffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create buffer!");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vkContext.device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = gpuResourceManager.findMemoryType(
      memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(vkContext.device, &allocInfo, nullptr, &bufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to allocate buffer memory!");
  }

  vkBindBufferMemory(vkContext.device, buffer, bufferMemory, 0);
}

void HelloTriangleApplication::createDescriptorSetLayout() {
  VkDescriptorSetLayoutBinding uboLayoutBinding = {};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags =
      VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr;  // Optional

  VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding samplerLayoutBinding2 = {};
  samplerLayoutBinding2.binding = 2;
  samplerLayoutBinding2.descriptorCount = 1;
  samplerLayoutBinding2.descriptorType =
      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding2.pImmutableSamplers = nullptr;
  samplerLayoutBinding2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
      uboLayoutBinding, samplerLayoutBinding, samplerLayoutBinding2};
  // std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding,
  // samplerLayoutBinding};
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  layoutInfo.pBindings = bindings.data();
  if (vkCreateDescriptorSetLayout(vkContext.device, &layoutInfo, nullptr,
                                  &descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor set layout!");
  }
}

void HelloTriangleApplication::createUniformBuffers() {
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);
  uniformBuffers.resize(windowContext.swapChainImages.size());
  uniformBuffersMemory.resize(windowContext.swapChainImages.size());

  for (size_t i = 0; i < windowContext.swapChainImages.size(); i++) {
    createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 uniformBuffers[i], uniformBuffersMemory[i]);
  }
}

void HelloTriangleApplication::createDescriptorPool() {
  std::array<VkDescriptorPoolSize, 3> poolSizes = {};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount =
      static_cast<uint32_t>(windowContext.swapChainImages.size());
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount =
      static_cast<uint32_t>(windowContext.swapChainImages.size());
  poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[2].descriptorCount =
      static_cast<uint32_t>(windowContext.swapChainImages.size());

  VkDescriptorPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets =
      static_cast<uint32_t>(windowContext.swapChainImages.size());

  if (vkCreateDescriptorPool(vkContext.device, &poolInfo, nullptr,
                             &descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create descriptor pool!");
  }
}

void HelloTriangleApplication::createDescriptorSets() {
  std::vector<VkDescriptorSetLayout> layouts(
      windowContext.swapChainImages.size(), descriptorSetLayout);
  VkDescriptorSetAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorPool;
  allocInfo.descriptorSetCount =
      static_cast<uint32_t>(windowContext.swapChainImages.size());
  allocInfo.pSetLayouts = layouts.data();

  descriptorSets.resize(windowContext.swapChainImages.size());
  if (vkAllocateDescriptorSets(vkContext.device, &allocInfo,
                               descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate descriptor sets!");
  }
  auto obj1Tex = gpuResourceManager.getTexture(obj1TexId);
  auto specImage = gpuResourceManager.getTexture(specTexId);

  for (size_t i = 0; i < windowContext.swapChainImages.size(); i++) {
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    VkDescriptorImageInfo imageInfo = {};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = obj1Tex.textureImageView;
    imageInfo.sampler = obj1Tex.textureSampler;

    VkDescriptorImageInfo specImageInfo = {};
    specImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    specImageInfo.imageView = specImage.textureImageView;
    specImageInfo.sampler = specImage.textureSampler;

    std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = descriptorSets[i];
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].dstArrayElement = 0;
    descriptorWrites[2].descriptorType =
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pImageInfo = &specImageInfo;

    vkUpdateDescriptorSets(vkContext.device,
                           static_cast<uint32_t>(descriptorWrites.size()),
                           descriptorWrites.data(), 0, nullptr);
  }
}

void HelloTriangleApplication::createObjectTextureImage() {
  obj1TexId = gpuResourceManager.createMyTexture(TEXTURE_PATH);
  specTexId = gpuResourceManager.createMyTexture(SPEC_TEXTURE_PATH);
}

void HelloTriangleApplication::createDepthResources() {
  VkFormat depthFormat = findDepthFormat();
  gpuResourceManager.createImage(
      windowContext.swapChainExtent.width, windowContext.swapChainExtent.height,
      1, depthFormat, VK_IMAGE_TILING_OPTIMAL,
      VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
  depthImageView = gpuResourceManager.createImageView(
      depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
  gpuResourceManager.transitionImageLayout(
      depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

void HelloTriangleApplication::loadObj1Model() {
  obj1 = gpuResourceManager.addModel(MODEL_PATH);
  gpuResourceManager.generateVkMeshBuffer(obj1);
}

void HelloTriangleApplication::initVulkan() {
  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  windowContext.window =
      glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  glfwSetWindowUserPointer(windowContext.window, this);
  glfwSetFramebufferSizeCallback(windowContext.window,
                                 framebufferResizeCallback);

  createResourceManager();
  createVkContext();
  createGpuResourceManager();
  gpuResourceManager.createSwapChain(windowContext);
  gpuResourceManager.createSwapChainImageViews(windowContext, 1);
  createRenderPass();
  createDescriptorSetLayout();
  createGraphicsPipeline();
  vkContext.createCommandPool();
  createDepthResources();
  createFramebuffers();
  loadObj1Model();
  createObjectTextureImage();
  // createVertexBuffer();
  // createIndexBuffer();
  createUniformBuffers();
  createDescriptorPool();
  createDescriptorSets();
  createCommandBuffers();
  createSyncObjects();
}

void HelloTriangleApplication::updateUniformBuffer(uint32_t currentImage) {
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();

  frameDeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                       currentTime - lastTime)
                       .count();
  lastTime = currentTime;

  // if (camXMoveSpeed != 0.0f) {
  //     camPosition += camDirect * camXMoveSpeed;
  // }
  UniformBufferObject ubo = {};
  ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
  //    ubo.model = glm::rotate(ubo.model, time * glm::radians(90.0f),
  //    glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = camera.GetViewMatrix();
  //    ubo.view = glm::translate(ubo.view, glm::vec3(time/sqrt(1), 0,
  //    time/sqrt(1)));
  ubo.proj = glm::perspective(glm::radians(45.f),
                              windowContext.swapChainExtent.width /
                                  (float)windowContext.swapChainExtent.height,
                              0.1f, 20.0f);

  // ubo.model[1][1] *= -1;
  ubo.proj[1][1] *= -1;

  ubo.viewPos = camera.Position;

  ubo.lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
  ubo.lightDiffuse = {0.250f, 0.235f, 0.168f};
  void *data;
  vkMapMemory(vkContext.device, uniformBuffersMemory[currentImage], 0,
              sizeof(ubo), 0, &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(vkContext.device, uniformBuffersMemory[currentImage]);
}

void HelloTriangleApplication::drawFrame() {
  vkWaitForFences(vkContext.device, 1, &inFlightFences[currentFrame], VK_TRUE,
                  UINT64_MAX);

  uint32_t imageIndex;
  VkResult vkResult = vkAcquireNextImageKHR(
      vkContext.device, windowContext.swapChain, UINT64_MAX,
      imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
  if (vkResult == VK_ERROR_OUT_OF_DATE_KHR) {
    recreateSwapChain();
    return;
  } else if (vkResult != VK_SUCCESS && vkResult != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(vkContext.device, 1, &imagesInFlight[imageIndex], VK_TRUE,
                    UINT64_MAX);
  }
  imagesInFlight[imageIndex] = inFlightFences[currentFrame];

  updateUniformBuffer(imageIndex);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

  VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  vkResetFences(vkContext.device, 1, &inFlightFences[currentFrame]);
  if (vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo,
                    inFlightFences[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;

  VkSwapchainKHR swapChains[] = {windowContext.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;

  presentInfo.pResults = nullptr;  // Optional
  vkResult = vkQueuePresentKHR(vkContext.presentQueue, &presentInfo);
  if (vkResult == VK_ERROR_OUT_OF_DATE_KHR || vkResult == VK_SUBOPTIMAL_KHR ||
      framebufferResized) {
    framebufferResized = false;
    recreateSwapChain();
  } else if (vkResult != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangleApplication::mainLoop() {
  // glfwSetKeyCallback(window, HelloTriangleApplication::keycallback_dispatch);
  glfwSetInputMode(windowContext.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(windowContext.window,
                           HelloTriangleApplication::mouse_callback_dispatch);

  while (!glfwWindowShouldClose(windowContext.window)) {
    processInput(windowContext.window);
    glfwPollEvents();
    drawFrame();
  }
  vkDeviceWaitIdle(vkContext.device);
}

void HelloTriangleApplication::cleanupSwapChain() {
  for (auto framebuffer : swapChainFramebuffers) {
    vkDestroyFramebuffer(vkContext.device, framebuffer, nullptr);
  }
  vkFreeCommandBuffers(vkContext.device, vkContext.commandPool,
                       static_cast<uint32_t>(commandBuffers.size()),
                       commandBuffers.data());
  vkDestroyPipeline(vkContext.device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(vkContext.device, pipelineLayout, nullptr);
  vkDestroyRenderPass(vkContext.device, renderPass, nullptr);
  for (auto imageView : windowContext.swapChainImageViews) {
    vkDestroyImageView(vkContext.device, imageView, nullptr);
  }
  for (size_t i = 0; i < windowContext.swapChainImages.size(); i++) {
    vkDestroyBuffer(vkContext.device, uniformBuffers[i], nullptr);
    vkFreeMemory(vkContext.device, uniformBuffersMemory[i], nullptr);
  }
  vkDestroyDescriptorPool(vkContext.device, descriptorPool, nullptr);
  vkDestroySwapchainKHR(vkContext.device, windowContext.swapChain, nullptr);
}

void HelloTriangleApplication::recreateSwapChain() {
  vkDeviceWaitIdle(vkContext.device);

  cleanupSwapChain();

  gpuResourceManager.createSwapChain(windowContext);
  gpuResourceManager.createSwapChainImageViews(windowContext, 1);
  createRenderPass();
  createGraphicsPipeline();
  createDepthResources();
  createFramebuffers();
  createUniformBuffers();
  createDescriptorPool();
  createDescriptorSets();
  createCommandBuffers();
}

void HelloTriangleApplication::cleanup() {
  cleanupSwapChain();

  gpuResourceManager.destoryTexture(obj1TexId);
  gpuResourceManager.destoryTexture(specTexId);
  vkDestroyDescriptorSetLayout(vkContext.device, descriptorSetLayout, nullptr);

  gpuResourceManager.destoryMesh(obj1);

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    vkDestroySemaphore(vkContext.device, renderFinishedSemaphores[i], nullptr);
    vkDestroySemaphore(vkContext.device, imageAvailableSemaphores[i], nullptr);
    vkDestroyFence(vkContext.device, inFlightFences[i], nullptr);
  }

  vkDestroyCommandPool(vkContext.device, vkContext.commandPool, nullptr);

  vkDestroyDevice(vkContext.device, nullptr);

  if (enableValidationLayers) {
    vkUtil::DestroyDebugUtilsMessengerEXT(vkContext.instance,
                                          vkContext.debugMessenger, nullptr);
  }

  vkDestroySurfaceKHR(vkContext.instance, vkContext.surface, nullptr);
  vkDestroyInstance(vkContext.instance, nullptr);

  // TODO
  glfwDestroyWindow(windowContext.window);

  glfwTerminate();
}

void HelloTriangleApplication::mouse_callback(GLFWwindow *window, double xpos,
                                              double ypos) {
  if (windowContext.firstMouse) {
    windowContext.lastX = xpos;
    windowContext.lastY = ypos;
    windowContext.firstMouse = false;
  }
  float xoffset = xpos - windowContext.lastX;
  float yoffset = windowContext.lastY - ypos;

  windowContext.lastX = xpos;
  windowContext.lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void HelloTriangleApplication::keycallback(GLFWwindow *window, int key,
                                           int scancode, int action, int mods) {
  // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  //   camera.ProcessKeyboard(FORWARD, frameDeltaTime);
  // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  //   camera.ProcessKeyboard(BACKWARD, frameDeltaTime);
  // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  //   camera.ProcessKeyboard(LEFT, frameDeltaTime);
  // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  //   camera.ProcessKeyboard(RIGHT, frameDeltaTime);

  // if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
  //   glfwSetInputMode(windowContext.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  // moveCount++;
  // std::cout << moveCount << std::endl;
}

void HelloTriangleApplication::processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, frameDeltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, frameDeltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, frameDeltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, frameDeltaTime);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.ProcessKeyboard(UP, frameDeltaTime);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.ProcessKeyboard(DOWN, frameDeltaTime);

  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    glfwSetInputMode(windowContext.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    glfwSetInputMode(windowContext.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // std::cout<< moveCount << std::endl;
}

void HelloTriangleApplication::run() {
  initVulkan();
  mainLoop();
  cleanup();
}