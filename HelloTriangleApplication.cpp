//
// Created by wicas on 2020/1/5.
//

#include "HelloTriangleApplication.h"

#include <functional>
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

HelloTriangleApplication *HelloTriangleApplication::event_handling_instance;

void HelloTriangleApplication::createResourceManager() {
  resourceManager = ResourceManager{};
}

// void HelloTriangleApplication::createGpuResourceManager() {
//   gpuResourceManager.initManager(&vkContext);
// }

// // TODO createSurface with several function
// void HelloTriangleApplication::createVkContext() {
//   vkContext.enableValidationLayers = enableValidationLayers;

//   uint32_t glfwExtensionCount = 0;
//   const char **glfwExtensions;
//   glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

//   volkInitialize();
//   vkContext.createInstance(vkUtil::getRequiredExtensions(
//       vkContext.enableValidationLayers, glfwExtensions, glfwExtensionCount));
//   volkLoadInstance(vkContext.instance);

//   createSurface();
//   vkContext.pickPhysicalDevice();
//   vkContext.createLogicalDevice();

//   vkContext.setupDebugMessenger();
// }

// void HelloTriangleApplication::createSurface() {
//   if (glfwCreateWindowSurface(vkContext.instance, windowContext.window,
//   nullptr,
//                               &vkContext.surface) != VK_SUCCESS) {
//     throw std::runtime_error("failed to create window surface!");
//   }
// }

// VkShaderModule HelloTriangleApplication::createShaderModule(
//     const std::vector<char> &code) {
//   VkShaderModuleCreateInfo createInfo = {};
//   createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//   createInfo.codeSize = code.size();
//   createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

//   VkShaderModule shaderModule;
//   if (vkCreateShaderModule(vkContext.device, &createInfo, nullptr,
//                            &shaderModule) != VK_SUCCESS) {
//     throw std::runtime_error("failed to create shader module!");
//   }

//   return shaderModule;
// }

void HelloTriangleApplication::createRenderGraph() {
  mRendergraph = crevice::RenderGraph();
  mRendergraph.setGpuRManager(renderServer.gpuRManager);

  // TODO rewrite rendergraph usage

  crevice::RGAttachment swapChainAtt{};
  swapChainAtt.externalTexture = true;
  swapChainAtt.name = "swapchain";
  swapChainAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  swapChainAtt.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  swapChainAtt.type = crevice::RGAttachmentTypes::Present;
  swapChainAtt.format = renderServer.swapChainFormat();
  swapId = mRendergraph.addAttachment(swapChainAtt);
  auto swapChainImgViews = renderServer.gpuRManager->createFRImageView(
      renderServer.windowContext->swapChainImageViews);
  mRendergraph.setExternalImageView(swapId, swapChainImgViews);

  crevice::RGAttachment depthAtt{};
  depthAtt.name = "depth";
  depthAtt.format = renderServer.findDepthFormat();
  depthAtt.externalTexture = true;
  depthAtt.type = crevice::RGAttachmentTypes::DepthStencil;
  depthAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAtt.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depId = mRendergraph.addAttachment(depthAtt);
  auto depViews = crevice::Vector<VkImageView>(renderServer.swapChainSize(),
                                               renderServer.windowContext->depthImageView);
  auto depImgViews = renderServer.gpuRManager->createFRImageView(depViews);
  mRendergraph.setExternalImageView(depId, depImgViews);

  // crevice::RGAttachment mainImage{};
  // mainImage.name = "mainImage";
  // mainImage.format = VK_FORMAT_B8G8R8A8_UNORM;
  // mainImage.type = crevice::RGAttachmentTypes::Color;
  // mainImage.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  // mainImage.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  // mRendergraph.addAttachment(mainImage);
}

void HelloTriangleApplication::setPassAndCompileRenderGraph() {
  using namespace crevice;

  crevice::RenderPass mainPass{};
  mainPass.changeName("mainPass");
  mainPass.changeShader("../../../../shaders/vert.spv",
                        "../../../../shaders/frag.spv");
  mainPass.outputAttachments = {swapId, depId};
  // Mesh infos
  auto bindDes = Vertex::getBindingDescription();
  mainPass.vertexInputInfoDesc.bindingDescription = {bindDes.stride};
  auto attributeDescriptions = Vertex::getAttributeDescriptions();
  for (auto att : attributeDescriptions) {
    mainPass.vertexInputInfoDesc.attributeDescriptions.push_back(
        {att.format, att.offset});
  }

  crevice::ShaderInputKey perpassInputKey{};
  perpassInputKey.keys = {Mat4x4, Mat4x4, Mat4x4, Vec3, Vec3, Vec3};
  mainPass.perPassInputKeys = {perpassInputKey};
  crevice::ShaderInputKey perObjInputKey{};
  perObjInputKey.keys = {TextureSample2D, TextureSample2D};
  mainPass.perObjInputKeys = {perObjInputKey};

  mainPassId =
      mRendergraph.addPass(crevice::make_shared<crevice::RenderPass>(mainPass));
  mRendergraph.setOutputPass(mainPassId);

  mRendergraph.compileWithSubPass();
}

void HelloTriangleApplication::createRenderAble() {
  using namespace crevice;

  auto setLayouts = mRendergraph.getDescriptorSetLayouts(mainPassId);

  ShaderInputKey perpassInputKey{};
  perpassInputKey.keys = {Mat4x4, Mat4x4, Mat4x4, Vec3, Vec3, Vec3};
  // auto perpasskeymap = crevice::getBindIngFromKey(perpassInputKey);
  PerPassRenderAble cameraRenderable{};
  cameraRenderable.key = perpassInputKey;
  cameraRenderable.passId = mainPassId;

  // create buffer descriptorSet
  cameraRenderable.bufferDescriptor =
      renderServer.gpuRManager->createFRDescriptorSet(
          *setLayouts[0], cameraUniformBuffers, sizeof(UniformBufferObject));

  // get mesh
  auto mesh1 = renderServer.gpuRManager->getById<Mesh>(obj1);
  auto meshRes = FrameResource<Mesh>(mesh1, GpuResourceManager::swapChainSize);
  // get mesh tex
  auto diffuseTex =
      renderServer.gpuRManager->getById<crevice::CVTexture>(obj1TexId);
  auto specTex =
      renderServer.gpuRManager->getById<crevice::CVTexture>(specTexId);

  // perpass
  // get perpass set layout
  // TODO this need to be constructed every time before draw,but can be diff
  RenderAble renderableObj1{};
  ShaderInputKey perobjInputKey{};
  perobjInputKey.keys = {TextureSample2D, TextureSample2D};
  renderableObj1.passId = mainPassId;
  renderableObj1.key = perobjInputKey;
  renderableObj1.mesh = meshRes;
  // TODO setLayout location
  renderableObj1.texDescriptor =
      renderServer.gpuRManager->createFRDescriptorSet(*setLayouts[1], {}, 0,
                                                      {diffuseTex, specTex});

  mRendergraph.updateRenderData({cameraRenderable}, {renderableObj1});
}

// VkFormat HelloTriangleApplication::findSupportedFormat(
//     const std::vector<VkFormat> &candidates, VkImageTiling tiling,
//     VkFormatFeatureFlags features) {
//   for (VkFormat format : candidates) {
//     VkFormatProperties props;
//     vkGetPhysicalDeviceFormatProperties(vkContext.physicalDevice, format,
//                                         &props);
//     if (tiling == VK_IMAGE_TILING_LINEAR &&
//         (props.linearTilingFeatures & features) == features) {
//       return format;
//     } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
//                (props.optimalTilingFeatures & features) == features) {
//       return format;
//     }
//   }
//   return VK_FORMAT_UNDEFINED;
// }

// VkFormat HelloTriangleApplication::findDepthFormat() {
//   return findSupportedFormat(
//       {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
//        VK_FORMAT_D24_UNORM_S8_UINT},
//       VK_IMAGE_TILING_OPTIMAL,
//       VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
// }

void HelloTriangleApplication::createPerPassUniformBuffers() {
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);
  auto swapChainSize = renderServer.swapChainSize();
  cameraUniformBuffers.resize(swapChainSize);
  cameraUniformBuffersMemory.resize(swapChainSize);

  for (size_t i = 0; i < swapChainSize; i++) {
    renderServer.gpuRManager->createBuffer(
        bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        cameraUniformBuffers[i], cameraUniformBuffersMemory[i]);
  }
}

// void HelloTriangleApplication::initDescriptorPool() {
//   gpuResourceManager.initDescriptorPool();
// }

void HelloTriangleApplication::createObjectTextureImage() {
  obj1TexId = renderServer.gpuRManager->createMyTexture(TEXTURE_PATH);
  specTexId = renderServer.gpuRManager->createMyTexture(SPEC_TEXTURE_PATH);
}

void HelloTriangleApplication::createDepthResources() {
  // VkFormat depthFormat = findDepthFormat();
  // gpuResourceManager.createImage(
  //     windowContext.swapChainExtent.width,
  //     windowContext.swapChainExtent.height, 1, depthFormat,
  //     VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
  //     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
  // depthImageView = gpuResourceManager.createImageView(
  //     depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
  // gpuResourceManager.transitionImageLayout(
  //     depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED,
  //     VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);

  renderServer.createSwapChainDepthResources();
}

void HelloTriangleApplication::loadObj1Model() {
  obj1 = renderServer.gpuRManager->addModel(MODEL_PATH);
  renderServer.gpuRManager->generateVkMeshBuffer(obj1);

  obj2 = renderServer.gpuRManager->addModel(APPLE_MODEL_PATH);
  renderServer.gpuRManager->generateVkMeshBuffer(obj2);
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
  ubo.proj = glm::perspective(
      glm::radians(45.f),
      renderServer.windowContext->swapChainExtent.width /
          (float)renderServer.windowContext->swapChainExtent.height,
      0.1f, 20.0f);

  // ubo.model[1][1] *= -1;
  ubo.proj[1][1] *= -1;

  ubo.viewPos = camera.Position;

  ubo.lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
  ubo.lightDiffuse = {0.250f, 0.235f, 0.168f};
  void *data;
  vkMapMemory(renderServer.vkContext->device,
              cameraUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0,
              &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(renderServer.vkContext->device,
                cameraUniformBuffersMemory[currentImage]);
}

// void HelloTriangleApplication::drawFrame() {
//   vkWaitForFences(renderServer.vkContext->device, 1,
//   &inFlightFences[currentFrame], VK_TRUE,
//                   UINT64_MAX);

//   uint32_t imageIndex;
//   VkResult vkResult = vkAcquireNextImageKHR(
//       vkContext.device, windowContext.swapChain, UINT64_MAX,
//       imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
//   if (vkResult == VK_ERROR_OUT_OF_DATE_KHR) {
//     recreateSwapChain();
//     return;
//   } else if (vkResult != VK_SUCCESS && vkResult != VK_SUBOPTIMAL_KHR) {
//     throw std::runtime_error("failed to acquire swap chain image!");
//   }

//   if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
//     vkWaitForFences(vkContext.device, 1, &imagesInFlight[imageIndex],
//     VK_TRUE,
//                     UINT64_MAX);
//   }
//   imagesInFlight[imageIndex] = inFlightFences[currentFrame];

//   updateUniformBuffer(imageIndex);

//   VkSubmitInfo submitInfo = {};
//   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

//   VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
//   VkPipelineStageFlags waitStages[] = {
//       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
//   submitInfo.waitSemaphoreCount = 1;
//   submitInfo.pWaitSemaphores = waitSemaphores;
//   submitInfo.pWaitDstStageMask = waitStages;

//   submitInfo.commandBufferCount = 1;
//   auto commandBuff =
//       gpuResourceManager.getById<CommandBuffers>(*drawingBuffersId);
//   submitInfo.pCommandBuffers = &commandBuff[imageIndex];

//   VkSemaphore renderFinishSemaphores[] = {
//       renderFinishedSemaphores[currentFrame]};
//   submitInfo.signalSemaphoreCount = 1;
//   submitInfo.pSignalSemaphores = renderFinishSemaphores;

//   vkResetFences(vkContext.device, 1, &inFlightFences[currentFrame]);
//   if (vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo,
//                     inFlightFences[currentFrame]) != VK_SUCCESS) {
//     throw std::runtime_error("failed to submit draw command buffer!");
//   }

//   VkPresentInfoKHR presentInfo = {};
//   presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

//   presentInfo.waitSemaphoreCount = 1;
//   presentInfo.pWaitSemaphores = renderFinishSemaphores;

//   VkSwapchainKHR swapChains[] = {windowContext.swapChain};
//   presentInfo.swapchainCount = 1;
//   presentInfo.pSwapchains = swapChains;
//   presentInfo.pImageIndices = &imageIndex;

//   presentInfo.pResults = nullptr;  // Optional
//   vkResult = vkQueuePresentKHR(vkContext.presentQueue, &presentInfo);
//   if (vkResult == VK_ERROR_OUT_OF_DATE_KHR || vkResult == VK_SUBOPTIMAL_KHR
//   ||
//       framebufferResized) {
//     framebufferResized = false;
//     recreateSwapChain();
//   } else if (vkResult != VK_SUCCESS) {
//     throw std::runtime_error("failed to present swap chain image!");
//   }

//   currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
// }

void HelloTriangleApplication::drawFrameWithFrameGraph() {
  updateUniformBuffer(currentFrame % GpuResourceManager::swapChainSize);
  mRendergraph.drawFrame(currentFrame);
  currentFrame = (currentFrame + 1);
}

void HelloTriangleApplication::mainLoop() {
  // glfwSetKeyCallback(window, HelloTriangleApplication::keycallback_dispatch);

  while (!container.windowShouldClose()) {
    container.processInputAndEvent();
    drawFrameWithFrameGraph();
  }
  renderServer.waitIdle();
}

void HelloTriangleApplication::cleanupSwapChain() {
  // vkDestroyImageView(renderServer.vkContext->device, depthImageView, nullptr);
  // vkDestroyImage(renderServer.vkContext->device, depthImage, nullptr);
  // vkFreeMemory(renderServer.vkContext->device, depthImageMemory, nullptr);

  for (auto framebuffer : swapChainFramebuffers) {
    vkDestroyFramebuffer(renderServer.vkContext->device, framebuffer, nullptr);
  }
  // gpuResourceManager.destoryCommandBuffers(commandBuffers2);
  // gpuResourceManager.destoryCommandBuffers(commandBuffers);

  vkDestroyPipeline(renderServer.vkContext->device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(renderServer.vkContext->device, pipelineLayout,
                          nullptr);
  vkDestroyRenderPass(renderServer.vkContext->device, renderPass, nullptr);
  for (auto imageView : renderServer.windowContext->swapChainImageViews) {
    vkDestroyImageView(renderServer.vkContext->device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(renderServer.vkContext->device,
                        renderServer.windowContext->swapChain, nullptr);
}

void HelloTriangleApplication::recreateSwapChain() {
  // TODO render pass,pipeline must recate in render graph
  // vkDeviceWaitIdle(vkContext.device);

  cleanupSwapChain();

  // gpuResourceManager.createSwapChain(windowContext);
  // gpuResourceManager.createSwapChainImageViews(windowContext, 1);
  // createRenderPass();
  // createGraphicsPipeline();
  createDepthResources();
  // createFramebuffers();
  // createCommandBuffers();
}

void HelloTriangleApplication::cleanup() {
  cleanupSwapChain();
  // vkDestroyDescriptorPool(vkContext.device,
  //                         gpuResourceManager.descriptorPools[0], nullptr);

  // for (size_t i = 0; i < windowContext.swapChainImages.size(); i++) {
  //   vkDestroyBuffer(vkContext.device, cameraUniformBuffers[i], nullptr);
  //   vkFreeMemory(vkContext.device, cameraUniformBuffersMemory[i], nullptr);
  // }
  // // TODO resource clear bucket;
  // gpuResourceManager.destroyTexture(obj1TexId);
  // gpuResourceManager.destroyTexture(specTexId);
  // // gpuResourceManager.destoryDescriptorSetLayout(*descriptorSetLayout);

  // gpuResourceManager.destroyMesh(obj1);
  // gpuResourceManager.destroyMesh(obj2);

  // // for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
  // //   vkDestroySemaphore(vkContext.device, renderFinishedSemaphores[i],
  // nullptr);
  // //   vkDestroySemaphore(vkContext.device, imageAvailableSemaphores[i],
  // nullptr);
  // //   vkDestroyFence(vkContext.device, inFlightFences[i], nullptr);
  // // }

  // vkDestroyCommandPool(vkContext.device, vkContext.commandPool, nullptr);

  // vkDestroyDevice(vkContext.device, nullptr);

  // if (enableValidationLayers) {
  //   vkUtil::DestroyDebugUtilsMessengerEXT(vkContext.instance,
  //                                         vkContext.debugMessenger, nullptr);
  // }

  // vkDestroySurfaceKHR(vkContext.instance, vkContext.surface, nullptr);
  // vkDestroyInstance(vkContext.instance, nullptr);

  // // TODO
  // glfwDestroyWindow(windowContext.window);

  // glfwTerminate();
}

void HelloTriangleApplication::mouse_callback(GLFWwindow *window, double xpos,
                                              double ypos) {
  // TODO this should in input server
  if (renderServer.windowContext->firstMouse) {
    renderServer.windowContext->lastX = xpos;
    renderServer.windowContext->lastY = ypos;
    renderServer.windowContext->firstMouse = false;
  }
  float xoffset = xpos - renderServer.windowContext->lastX;
  float yoffset = renderServer.windowContext->lastY - ypos;

  renderServer.windowContext->lastX = xpos;
  renderServer.windowContext->lastY = ypos;

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
    switchCommandBuffer(&commandBuffers2);
  }
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
    switchCommandBuffer(&commandBuffers);
  }
}

void HelloTriangleApplication::switchCommandBuffer(RID *bufferId) {
  drawingBuffersId = bufferId;
}

void HelloTriangleApplication::initVulkan() {
  // glfwInit();

  // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  // windowContext.window =
  //     glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  // glfwSetWindowUserPointer(windowContext.window, this);
  // glfwSetFramebufferSizeCallback(windowContext.window,
  //                                framebufferResizeCallback);
  // glfwSetInputMode(windowContext.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  // glfwSetCursorPosCallback(windowContext.window,
  //                          HelloTriangleApplication::mouse_callback_dispatch);

  // createResourceManager();
  // createVkContext();
  // // TODO more ealgent Bind;
  // vkContext.windowContext = &windowContext;
  // createGpuResourceManager();
  // gpuResourceManager.createSwapChain(windowContext);
  // gpuResourceManager.createSwapChainImageViews(windowContext, 1);
  // vkContext.createCommandPool();
  createDepthResources();

  createRenderGraph();

  // TODO create Renderable and perpassRenderable, this process is all about io,
  // so this can be async
  loadObj1Model();
  createObjectTextureImage();
  createPerPassUniformBuffers();
  // initDescriptorPool();
  // createDescriptorSets();

  setPassAndCompileRenderGraph();
  // createCommandBuffers();
  // set drawCommand id
  // drawingBuffersId = &commandBuffers;
  // createSyncObjects();

  createRenderAble();
}

void HelloTriangleApplication::serverSetup() {
  container = crevice::GLFWContainer(WIDTH, HEIGHT, "Vulkan");
  {
    using namespace std::placeholders;
    container.inputProcessor = std::bind(
        &HelloTriangleApplication::processInput, this, std::placeholders::_1);
    container.framebufferResizeCallback =
        &HelloTriangleApplication::framebufferResizeCallback;
    container.mouse_callback_dispatch = &HelloTriangleApplication::mouse_callback_dispatch;
  }
  container.setUpInput(this);
  renderServer = crevice::RenderServer();
  renderServer.setContainer(&container);
  renderServer.init();
}

void HelloTriangleApplication::run() {
  serverSetup();
  initVulkan();
  mainLoop();
  cleanup();
}
