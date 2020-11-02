//
// Created by wicas on 2020/1/5.
//

#include "HelloTriangleApplication.h"

#include <functional>
#include <unordered_map>

const int WIDTH = 800;
const int HEIGHT = 600;

const std::string MODEL_PATH = "../../../../models/monkey.obj";
const std::string MODEL2_PATH = "../../../../models/box2.obj";
const std::string APPLE_MODEL_PATH = "../../../../models/apple.obj";
const std::string TEXTURE_PATH = "../../../../textures/badApple.png";
const std::string SPEC_TEXTURE_PATH =
    "../../../../textures/container2_specular.png";

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

HelloTriangleApplication *HelloTriangleApplication::event_handling_instance;

void HelloTriangleApplication::createResourceManager()
{
  resourceManager = ResourceManager{};
}

void HelloTriangleApplication::createRenderGraph()
{
  mRendergraph = crevice::RenderGraph();
  mRendergraph.setGpuRManager(renderServer->gpuRManager);

  // TODO rewrite rendergraph usage

  crevice::RGAttachment swapChainAtt{};
  swapChainAtt.externalTexture = true;
  swapChainAtt.name = "swapchain";
  swapChainAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  swapChainAtt.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  swapChainAtt.type = crevice::RGAttachmentTypes::Present;
  swapChainAtt.format = renderServer->swapChainFormat();
  swapId = mRendergraph.addAttachment(swapChainAtt);
  auto swapChainImgViews = renderServer->gpuRManager->createFRImageView(
      renderServer->windowContext->swapChainImageViews);
  mRendergraph.setExternalImageView(swapId, swapChainImgViews);

  crevice::RGAttachment depthAtt{};
  depthAtt.name = "depth";
  depthAtt.format = renderServer->findDepthFormat();
  depthAtt.externalTexture = true;
  depthAtt.type = crevice::RGAttachmentTypes::DepthStencil;
  depthAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAtt.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depId = mRendergraph.addAttachment(depthAtt);
  auto depViews = crevice::Vector<VkImageView>(
      renderServer->swapChainSize(), renderServer->windowContext->depthImageView);
  auto depImgViews = renderServer->gpuRManager->createFRImageView(depViews);
  mRendergraph.setExternalImageView(depId, depImgViews);

  // crevice::RGAttachment mainImage{};
  // mainImage.name = "mainImage";
  // mainImage.format = VK_FORMAT_B8G8R8A8_UNORM;
  // mainImage.type = crevice::RGAttachmentTypes::Color;
  // mainImage.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  // mainImage.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  // mRendergraph.addAttachment(mainImage);
}

void HelloTriangleApplication::setPassAndCompileRenderGraph()
{
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
  for (auto att : attributeDescriptions)
  {
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

void HelloTriangleApplication::createRenderAble()
{
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
      renderServer->gpuRManager->createFRDescriptorSet(
          *setLayouts[0], cameraUniformBuffers, sizeof(UniformBufferObject));

  // get mesh
  auto mesh1 = renderServer->gpuRManager->getById<Mesh>(obj1);
  auto meshRes = FrameResource<Mesh>(mesh1, GpuResourceManager::swapChainSize);
  // get mesh tex
  auto diffuseTex =
      renderServer->gpuRManager->getById<crevice::CVTexture>(obj1TexId);
  auto specTex =
      renderServer->gpuRManager->getById<crevice::CVTexture>(specTexId);

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
      renderServer->gpuRManager->createFRDescriptorSet(*setLayouts[1], {}, 0,
                                                       {diffuseTex, specTex});

  mRendergraph.updateRenderData({cameraRenderable}, {renderableObj1});
}

void HelloTriangleApplication::createPerPassUniformBuffers()
{
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);
  auto swapChainSize = renderServer->swapChainSize();
  cameraUniformBuffers.resize(swapChainSize);
  cameraUniformBuffersMemory.resize(swapChainSize);

  for (size_t i = 0; i < swapChainSize; i++)
  {
    renderServer->gpuRManager->createBuffer(
        bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        cameraUniformBuffers[i], cameraUniformBuffersMemory[i]);
  }
}

void HelloTriangleApplication::createObjectTextureImage()
{
  obj1TexId = renderServer->gpuRManager->createMyTexture(TEXTURE_PATH);
  specTexId = renderServer->gpuRManager->createMyTexture(SPEC_TEXTURE_PATH);
}

void HelloTriangleApplication::createDepthResources()
{
  renderServer->createSwapChainDepthResources();
}

void HelloTriangleApplication::loadObj1Model()
{
  obj1 = renderServer->gpuRManager->addModel(MODEL_PATH);
  renderServer->gpuRManager->generateVkMeshBuffer(obj1);

  obj2 = renderServer->gpuRManager->addModel(APPLE_MODEL_PATH);
  renderServer->gpuRManager->generateVkMeshBuffer(obj2);
}

void HelloTriangleApplication::updateUniformBuffer(uint32_t currentImage)
{
  static auto startTime = std::chrono::high_resolution_clock::now();
  auto currentTime = std::chrono::high_resolution_clock::now();

  frameDeltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                       currentTime - lastTime)
                       .count();

  auto timepast = std::chrono::duration<float, std::chrono::seconds::period>(
                      startTime - currentTime)
                      .count();
  lastTime = currentTime;

  // if (camXMoveSpeed != 0.0f) {
  //     camPosition += camDirect * camXMoveSpeed;
  // }
  UniformBufferObject ubo = {};
  //TODO setup uniform in ecs

  glm::vec3 pos = {0.0f, 0.0f, -5.0f};

  // auto r = static_cast<float>(currentTime) * 1.f;
  glm::vec3 rotation = {45.f, 100.0f, 0.0f};
  glm::vec3 scale = {1.f, 1.0f, 1.0f};

  glm::mat4 model = glm::mat4(1.f);
  model = glm::translate(model, pos);
  auto q = glm::quat(glm::radians(rotation));
  model *= glm::mat4_cast(q);
  model = glm::scale(model, scale);

  ubo.model = model;

  // ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
  //    ubo.model = glm::rotate(ubo.model, time * glm::radians(90.0f),
  //    glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = camera.GetViewMatrix();
  //    ubo.view = glm::translate(ubo.view, glm::vec3(time/sqrt(1), 0,
  //    time/sqrt(1)));
  ubo.proj = glm::perspective(
      glm::radians(45.f),
      renderServer->windowContext->swapChainExtent.width /
          (float)renderServer->windowContext->swapChainExtent.height,
      0.1f, 20.0f);

  ubo.proj[1][1] *= -1;

  ubo.viewPos = camera.Position;

  ubo.lightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
  ubo.lightDiffuse = {0.250f, 0.235f, 0.168f};

  //TODO update uniform buffer to certain pass
  void *data;
  vkMapMemory(renderServer->vkContext->device,
              cameraUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0,
              &data);
  memcpy(data, &ubo, sizeof(ubo));
  vkUnmapMemory(renderServer->vkContext->device,
                cameraUniformBuffersMemory[currentImage]);
}

void HelloTriangleApplication::drawFrameWithFrameGraph()
{
  updateUniformBuffer(currentFrame % GpuResourceManager::swapChainSize);
  mRendergraph.drawFrame(currentFrame);
  currentFrame = (currentFrame + 1);
}

void HelloTriangleApplication::mainLoop()
{
  // glfwSetKeyCallback(window, HelloTriangleApplication::keycallback_dispatch);

  while (!container.windowShouldClose())
  {
    container.processInputAndEvent();
    drawFrameWithFrameGraph();
  }
  renderServer->waitIdle();
}

void HelloTriangleApplication::cleanupSwapChain()
{
  // vkDestroyImageView(renderServer->vkContext->device, depthImageView,
  // nullptr); vkDestroyImage(renderServer->vkContext->device, depthImage,
  // nullptr); vkFreeMemory(renderServer->vkContext->device, depthImageMemory,
  // nullptr);

  for (auto framebuffer : swapChainFramebuffers)
  {
    vkDestroyFramebuffer(renderServer->vkContext->device, framebuffer, nullptr);
  }
  // gpuResourceManager.destoryCommandBuffers(commandBuffers2);
  // gpuResourceManager.destoryCommandBuffers(commandBuffers);

  vkDestroyPipeline(renderServer->vkContext->device, graphicsPipeline, nullptr);
  vkDestroyPipelineLayout(renderServer->vkContext->device, pipelineLayout,
                          nullptr);
  vkDestroyRenderPass(renderServer->vkContext->device, renderPass, nullptr);
  for (auto imageView : renderServer->windowContext->swapChainImageViews)
  {
    vkDestroyImageView(renderServer->vkContext->device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(renderServer->vkContext->device,
                        renderServer->windowContext->swapChain, nullptr);
}

void HelloTriangleApplication::recreateSwapChain()
{
  // TODO render pass,pipeline must recate in render graph
  // vkDeviceWaitIdle(vkContext.device);

  cleanupSwapChain();

  // gpuResourceManager.createSwapChain(windowContext);
  // gpuResourceManager.createSwapChainImageViews(windowContext, 1);
  // createRenderPass();
  // createGraphicsPipeline();
  // createDepthResources();
  // createFramebuffers();
  // createCommandBuffers();
}

void HelloTriangleApplication::cleanup()
{
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
                                              double ypos)
{
  // TODO this should in input server
  if (renderServer->windowContext->firstMouse)
  {
    renderServer->windowContext->lastX = xpos;
    renderServer->windowContext->lastY = ypos;
    renderServer->windowContext->firstMouse = false;
  }
  float xoffset = xpos - renderServer->windowContext->lastX;
  float yoffset = renderServer->windowContext->lastY - ypos;

  renderServer->windowContext->lastX = xpos;
  renderServer->windowContext->lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void HelloTriangleApplication::processInput(GLFWwindow *window)
{
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

  // if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
  //   switchCommandBuffer(&commandBuffers2);
  // }
  // if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
  //   switchCommandBuffer(&commandBuffers);
  // }
}

// void HelloTriangleApplication::switchCommandBuffer(RID *bufferId) {
//   drawingBuffersId = bufferId;
// }

void HelloTriangleApplication::initVulkan()
{

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

void HelloTriangleApplication::serverSetup()
{
  container = crevice::GLFWContainer(WIDTH, HEIGHT, "Vulkan");
  {
    using namespace std::placeholders;
    container.inputProcessor = std::bind(
        &HelloTriangleApplication::processInput, this, std::placeholders::_1);
    container.framebufferResizeCallback =
        &HelloTriangleApplication::framebufferResizeCallback;
    container.mouse_callback_dispatch =
        &HelloTriangleApplication::mouse_callback_dispatch;
  }
  container.setUpInput(this);
  renderServer = crevice::RenderServer::getInstance();
  renderServer->setContainer(&container);
  renderServer->init();
}

void HelloTriangleApplication::setupECS()
{
  //add persistant entities

  //add systems for update
}

void HelloTriangleApplication::run()
{
  serverSetup();
  initVulkan();
  setupECS();
  mainLoop();
  cleanup();
}

// std::chrono::high_resolution_clock::time_point HelloTriangleApplication::startTime = std::chrono::high_resolution_clock::now()