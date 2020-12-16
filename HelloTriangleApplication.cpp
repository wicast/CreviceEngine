//
// Created by wicas on 2020/1/5.
//

#include "HelloTriangleApplication.h"

#include "common/InputState.h"
#include "systems/default_render_sys.h"
#include "systems_cust/cust_system.h"
#include "systems/sys_asset_loader.hpp"
#include "components/ecs_register_comp.h"

const int WIDTH = 800;
const int HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

HelloTriangleApplication* HelloTriangleApplication::event_handling_instance;

void HelloTriangleApplication::mainLoop() {
  // glfwSetKeyCallback(window, HelloTriangleApplication::keycallback_dispatch);

  while (!container.windowShouldClose()) {
    crevice::CVTimer::tick();
    container.processInputAndEvent();
    ecs.progress();
  }
  renderServer->waitIdle();
}

void HelloTriangleApplication::cleanupSwapChain() {
  // TODO cleanup resource
  // vkDestroyImageView(renderServer->vkContext->device, depthImageView,
  // nullptr); vkDestroyImage(renderServer->vkContext->device, depthImage,
  // nullptr); vkFreeMemory(renderServer->vkContext->device, depthImageMemory,
  // nullptr);

  // for (auto framebuffer : swapChainFramebuffers) {
  //   vkDestroyFramebuffer(renderServer->vkContext->device, framebuffer,
  //   nullptr);
  // }

  // vkDestroyPipeline(renderServer->vkContext->device, graphicsPipeline,
  // nullptr); vkDestroyPipelineLayout(renderServer->vkContext->device,
  // pipelineLayout,
  //                         nullptr);
  // vkDestroyRenderPass(renderServer->vkContext->device, renderPass, nullptr);
  // for (auto imageView : renderServer->windowContext->swapChainImageViews) {
  //   vkDestroyImageView(renderServer->vkContext->device, imageView, nullptr);
  // }

  // vkDestroySwapchainKHR(renderServer->vkContext->device,
  //                       renderServer->windowContext->swapChain, nullptr);
}

void HelloTriangleApplication::recreateSwapChain() {
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

void HelloTriangleApplication::cleanup() {
  cleanupSwapChain();
  // TODO resource clear in resource manager;

  // vkDestroyDescriptorPool(vkContext.device,
  //                         gpuResourceManager.descriptorPools[0], nullptr);

  // for (size_t i = 0; i < windowContext.swapChainImages.size(); i++) {
  //   vkDestroyBuffer(vkContext.device, cameraUniformBuffers[i], nullptr);
  //   vkFreeMemory(vkContext.device, cameraUniformBuffersMemory[i], nullptr);
  // }
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

  // glfwDestroyWindow(windowContext.window);

  // glfwTerminate();
}

void HelloTriangleApplication::mouse_callback(GLFWwindow* window,
                                              double xpos,
                                              double ypos) {
  // TODO this should in input server
  // if (renderServer->windowContext->firstMouse)
  // {
  //   renderServer->windowContext->lastX = xpos;
  //   renderServer->windowContext->lastY = ypos;
  //   renderServer->windowContext->firstMouse = false;
  // }
  // float xoffset = xpos - renderServer->windowContext->lastX;
  // float yoffset = renderServer->windowContext->lastY - ypos;

  // renderServer->windowContext->lastX = xpos;
  // renderServer->windowContext->lastY = ypos;

  // camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void HelloTriangleApplication::processInput(GLFWwindow* window) {
  crevice::InputState::states.clear();
  //TODO key mapping
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_ESCAPE);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_W);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_S);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_A);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_D);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_E);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_Q);

  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_N);
  if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
    crevice::InputState::states.emplace(GLFW_KEY_I);
}

void HelloTriangleApplication::serverSetup() {
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

  assetManager = new AssetManager();
  rManager = new ResourceManager();

  assetResourceBridge = AssetResourceBridge::getInstance();
  assetResourceBridge->setup(rManager, assetManager);

  vkResourceBridge = crevice::VkResourceBridge::getInstance();
  vkResourceBridge->setup(rManager, renderServer->gpuRManager, renderServer->vkContext);
}

void HelloTriangleApplication::setupECS() {
  ecs_reg_comp(ecs);
  // ecs.set_target_fps(144);
  SetRenderHandler(ecs);

  ecs.system<Camera>(nullptr, "$RenderHandler")
      .kind(flecs::OnSet)
      .iter(setupPerpassRenderAble);

  // ecs.system<Transform, MeshInfoComp, MaterialInfoComp>(nullptr, "$RenderHandler")
  //     .kind(flecs::OnSet)
  //     .iter(loadPerObjRenderAbleAsset);

  ecs.system<Transform, VkMeshRes, MaterialInfoComp, VkTextureResComp>(nullptr, "$RenderHandler")
      .kind(flecs::OnSet)
      .iter(updatePerObjRenderAbleDescriptor);

  setAssetResourceSystems(ecs);

  // add systems for update

  ecs.system<Camera>().each(updateCamera);

  ecs.system<Transform>().each(updateTransform);

  ecs.system<Camera>(nullptr, "$RenderHandler").iter(updatePerpassRenderAble);

  ecs.system<Transform>(nullptr, "$RenderHandler").iter(updateModelUniform);

  ecs.system<>(nullptr, "$RenderHandler").iter(updateUboBuffer);

  ecs.system<>(nullptr, "$RenderHandler").iter(increaseCurrentFrame);

  // add persistant entities
  auto cam = ecs.entity();
  cam.set<Camera>(Camera());

  auto obj = ecs.entity();
  Transform t{};
  t.position = {0.0f, 0.0f, -5.0f};
  obj.set<Transform>(t);

  //TODO get rid from asset
  obj.set<MeshInfoComp>(MeshInfoComp{5,true,true});
  obj.set<MaterialInfoComp>(MaterialInfoComp{3,4,true,true,false});
}

void HelloTriangleApplication::run() {
  serverSetup();
  // initVulkan();
  setupECS();
  mainLoop();
  cleanup();
}