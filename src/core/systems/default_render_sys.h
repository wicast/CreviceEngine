/**
 * @file default_render_sys.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include "flecs.h"

#include "common/CVTimer.h"
#include "common/CamControl.h"
#include "components/Camera.h"
#include "components/resource/MaterialInfoComp.h"
#include "components/resource/MeshInfoComp.h"
#include "components/resource/VkMeshRes.hpp"
#include "components/resource/VkTextureResComp.hpp"
#include "components/RenderHandler.h"
#include "components/Transform.h"
#include "graphic/vk_render/Uniform.h"
#include "graphic/vk_render/utils/vkUtils.hpp"

#include "graphic/vk_render/RenderServer.h"

#include "common/InputState.h"

/**
 * @brief Set the Render Handler object
 * 
 * @param world 
 */
void SetRenderHandler(flecs::world& world) {
  using namespace crevice;
  auto mRendergraph = new RenderGraph();
  auto renderServer = RenderServer::getInstance();
  auto gpuRManager = renderServer->gpuRManager;
  mRendergraph->setGpuRManager(gpuRManager);
  mRendergraph->setWindowContext(renderServer->defaultWindowContext);
  

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
      renderServer->defaultWindowContext->swapChainImageViews);
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
      crevice::Vector<VkImageView>(renderServer->defaultWindowContext->swapChainSize,
                                   renderServer->defaultWindowContext->depthImageView);
  auto depImgViews = gpuRManager->createFRImageView(depViews);
  mRendergraph->setExternalImageView(depId, depImgViews);

  // TODO default renderpass, this should have various setup
  crevice::RenderPass mainPass{};
  mainPass.changeName("mainPass");
  mainPass.changeShader("../../../../shaders/vert.spv",
                        "../../../../shaders/frag.spv");
  mainPass.outputAttachments = {swapId, depId};
  // Mesh infos
  auto bindDes = getBindingDescription<Vertex>();
  mainPass.vertexInputInfoDesc.bindingDescription = {bindDes.stride};
  auto attributeDescriptions = getAttributeDescriptions();
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

/**
 * @brief Query with $RenderHandler
 * 
 * @param it 
 * @param cam 
 */
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
      renderServer->gpuRManager->createFRDescriptorSet( renderServer->defaultWindowContext->swapChainSize,
          *setLayouts[0], renderHandler->cameraUniformBuffers,
          sizeof(UniformBufferObject));

  renderHandler->mainRendergraph->updateRenderData({cameraRenderable}, {});
}

/**
 * @brief Query with $RenderHandler
 * 
 * @param it 
 * @param cams 
 */
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

/**
 * @brief Query with $RenderHandler
 * 
 * @param it 
 * @param t 
 */
void updateModelUniform(flecs::iter it, Transform t[]) {
  using namespace crevice;

  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(2);

  for (auto i : it) {
    /* code */
    renderHandler->ubo.model = t[i].getModelMatrix();
  }
}

/**
 * @brief  This is not perframe \n
 * Query with $RenderHandler
 * @param it 
 * @param objs 
 * @param meshes 
 * @param mats 
 */
void updatePerObjRenderAbleDescriptor(flecs::iter it,
                                      Transform objs[],
                                      VkMeshRes meshes[],
                                      MaterialInfoComp mats[],
                                      VkTextureResComp texs[]) {
  // TODO load assets
  using namespace crevice;
  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(5);

  auto setLayouts = renderHandler->mainRendergraph->getDescriptorSetLayouts(
      renderHandler->mainPassId);

  Vector<RenderAble> renderList;
  for (auto i : it) {
    auto mat = mats[i];
    if (!mat.loaded)
    {
      /* code */
      continue;
    }
    
    // get mesh
    auto mesh1 = meshes[i].mesh;
    auto meshRes =
        FrameResource<VkMesh>(mesh1, renderServer->defaultWindowContext->swapChainSize);
    // get mesh tex
    auto diffuseTex =
        *(texs[i].textureLoaded[mat.obj1TexId]);
    auto specTex =
        *(texs[i].textureLoaded[mat.specTexId]);

    RenderAble renderableObj{};
    ShaderInputKey perobjInputKey{};
    perobjInputKey.keys = {TextureSample2D, TextureSample2D};
    renderableObj.passId = renderHandler->mainPassId;

    renderableObj.key = perobjInputKey;
    renderableObj.mesh = meshRes;
    // TODO setLayout location
    renderableObj.texDescriptor =
        renderServer->gpuRManager->createFRDescriptorSet(renderServer->defaultWindowContext->swapChainSize,*setLayouts[1], {}, 0,
                                                         {diffuseTex, specTex});

    renderList.push_back(renderableObj);
  }

  // perpass
  // get perpass set layout
  // TODO this need to be constructed every time before draw,but can be diff
  renderHandler->mainRendergraph->updateRenderData({}, renderList);
}

/**
 * @brief Query with $RenderHandler
 * 
 * @param it 
 */
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

/**
 * @brief Query with $RenderHandler
 * 
 * @param it 
 */
void increaseCurrentFrame(flecs::iter it) {
  using namespace crevice;
  auto renderServer = RenderServer::getInstance();
  auto renderHandler = it.column<RenderHandler>(1);

  renderHandler->mainRendergraph->drawFrame(renderHandler->currentFrame);

  renderHandler->currentImage =
      (renderHandler->currentFrame) % renderServer->swapChainSize();
  renderHandler->currentFrame++;
}