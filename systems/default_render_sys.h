#include "flecs.h"

#include "common/CVTimer.h"
#include "common/CamControl.h"
#include "components/Camera.h"
#include "components/MaterialHandle.h"
#include "components/MeshHandler.h"
#include "components/RenderHandler.h"
#include "components/Transform.h"
#include "render/Uniform.h"

#include "render/RenderServer.h"

#include "common/InputState.h"

void SetRenderHandler(flecs::world& world) {
  using namespace crevice;
  auto mRendergraph = new RenderGraph();
  auto renderServer = RenderServer::getInstance();
  auto gpuRManager = renderServer->gpuRManager;
  mRendergraph->setGpuRManager(gpuRManager);

  // TODO rewrite rendergraph usage

  // TODO default attachments
  crevice::RGAttachment swapChainAtt{};
  swapChainAtt.externalTexture = true;
  swapChainAtt.name = "swapchain";
  swapChainAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  swapChainAtt.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  swapChainAtt.type = crevice::RGAttachmentTypes::Present;
  swapChainAtt.format = renderServer->swapChainFormat();
  auto swapId = mRendergraph->addAttachment(swapChainAtt);
  auto swapChainImgViews = gpuRManager->createFRImageView(
      renderServer->windowContext->swapChainImageViews);
  mRendergraph->setExternalImageView(swapId, swapChainImgViews);

  crevice::RGAttachment depthAtt{};
  depthAtt.name = "depth";
  depthAtt.format = renderServer->findDepthFormat();
  depthAtt.externalTexture = true;
  depthAtt.type = crevice::RGAttachmentTypes::DepthStencil;
  depthAtt.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAtt.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  auto depId = mRendergraph->addAttachment(depthAtt);
  auto depViews =
      crevice::Vector<VkImageView>(renderServer->swapChainSize(),
                                   renderServer->windowContext->depthImageView);
  auto depImgViews = gpuRManager->createFRImageView(depViews);
  mRendergraph->setExternalImageView(depId, depImgViews);

  // TODO default renderpass, this should have various setup
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

  auto mainPassId = mRendergraph->addPass(
      crevice::make_shared<crevice::RenderPass>(mainPass));
  mRendergraph->setOutputPass(mainPassId);

  mRendergraph->compileWithSubPass();

  // create depth resource

  RenderHandler renderHandler{};

  renderHandler.mainPassId = mainPassId;
  renderHandler.mainRendergraph = mRendergraph;

  // TODO this buffer still need be created when needed
  // camuniform buffer setup
  VkDeviceSize bufferSize = sizeof(UniformBufferObject);
  auto swapChainSize = renderServer->swapChainSize();
  renderHandler.cameraUniformBuffers.resize(swapChainSize);
  renderHandler.cameraUniformBuffersMemory.resize(swapChainSize);

  for (size_t i = 0; i < swapChainSize; i++) {
    renderServer->gpuRManager->createBuffer(
        bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        renderHandler.cameraUniformBuffers[i],
        renderHandler.cameraUniformBuffersMemory[i]);
  }

  world.set<RenderHandler>(renderHandler);
}

// Query with $RenderHandler
void setupPerpassRenderAble(flecs::iter it, Camera cam[]) {
  using namespace crevice;

  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(2);

  ShaderInputKey perpassInputKey{};
  perpassInputKey.keys = {Mat4x4, Mat4x4, Mat4x4, Vec3, Vec3, Vec3};

  PerPassRenderAble cameraRenderable{};
  cameraRenderable.key = perpassInputKey;
  cameraRenderable.passId = renderHandler->mainPassId;

  auto setLayouts = renderHandler->mainRendergraph->getDescriptorSetLayouts(
      renderHandler->mainPassId);

  // create buffer descriptorSet
  cameraRenderable.bufferDescriptor =
      renderServer->gpuRManager->createFRDescriptorSet(
          *setLayouts[0], renderHandler->cameraUniformBuffers,
          sizeof(UniformBufferObject));

  renderHandler->mainRendergraph->updateRenderData({cameraRenderable}, {});
}

// Query with $RenderHandler
void updatePerpassRenderAble(flecs::iter it, Camera cams[]) {
  using namespace crevice;

  // auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(2);

  for (auto i : it) {
    auto cam = cams[i];
    renderHandler->ubo.view = cam.GetViewMatrix();
    renderHandler->ubo.proj = cam.GetProjMatrix();

    renderHandler->ubo.lightPosition = {5.0f, 5.0f, 5.0f};
    renderHandler->ubo.lightDiffuse = {0.250f, 0.235f, 0.168f};
  }
}

// Query with $RenderHandler
void updateModelUniform(flecs::iter it, Transform t[]) {
  using namespace crevice;

  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(2);

  for (auto i : it) {
    /* code */
    renderHandler->ubo.model = t[i].getModelMatrix();
  }
}

// This is not perframe
// Query with $RenderHandler
void updatePerObjRenderAbleDescriptor(flecs::iter it,
                                      Transform objs[],
                                      MeshHandler meshes[],
                                      MaterialHandle mats[]) {
  // TODO load assets
  using namespace crevice;
  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(4);

  auto setLayouts = renderHandler->mainRendergraph->getDescriptorSetLayouts(
      renderHandler->mainPassId);

  Vector<RenderAble> renderList;
  for (auto i : it) {
    /* code */
    // get mesh
    auto mesh1 = renderServer->gpuRManager->getById<Mesh>(meshes[i].meshId);
    auto meshRes =
        FrameResource<Mesh>(mesh1, GpuResourceManager::swapChainSize);
    // get mesh tex
    auto mat = mats[i];
    auto diffuseTex =
        renderServer->gpuRManager->getById<crevice::CVTexture>(mat.obj1TexId);
    auto specTex =
        renderServer->gpuRManager->getById<crevice::CVTexture>(mat.specTexId);

    RenderAble renderableObj{};
    ShaderInputKey perobjInputKey{};
    perobjInputKey.keys = {TextureSample2D, TextureSample2D};
    renderableObj.passId = renderHandler->mainPassId;

    renderableObj.key = perobjInputKey;
    renderableObj.mesh = meshRes;
    // TODO setLayout location
    renderableObj.texDescriptor =
        renderServer->gpuRManager->createFRDescriptorSet(*setLayouts[1], {}, 0,
                                                         {diffuseTex, specTex});

    renderList.push_back(renderableObj);
  }

  // perpass
  // get perpass set layout
  // TODO this need to be constructed every time before draw,but can be diff
  renderHandler->mainRendergraph->updateRenderData({}, renderList);
}

// Query with $RenderHandler
void updateUboBuffer(flecs::iter it) {
  using namespace crevice;

  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(1);

  void* data;
  vkMapMemory(
      renderServer->vkContext->device,
      renderHandler->cameraUniformBuffersMemory[renderHandler->currentImage], 0,
      sizeof(renderHandler->ubo), 0, &data);
  memcpy(data, &renderHandler->ubo, sizeof(renderHandler->ubo));
  vkUnmapMemory(
      renderServer->vkContext->device,
      renderHandler->cameraUniformBuffersMemory[renderHandler->currentImage]);
}

// Query with $RenderHandler
void increaseCurrentFrame(flecs::iter it) {
  using namespace crevice;
  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(1);

  renderHandler->mainRendergraph->drawFrame(renderHandler->currentFrame);

  renderHandler->currentImage =
      (renderHandler->currentFrame) % renderServer->swapChainSize();
  renderHandler->currentFrame++;
}