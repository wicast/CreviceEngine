/**
 * @file RenderGraph.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "render/FrameResource.h"
#include "render/RenderAble.h"
#include "render/renderGraph/RGAttachment.h"
#include "render/renderGraph/RenderPass.h"
#include "stl/CreviceDeque.h"
#include "stl/CreviceHashMap.h"
#include "stl/CreviceHashSet.h"
#include "stl/CreviceSharedPtr.h"
#include "stl/CreviceVector.h"
#include "stl/CreviceVectorMap.h"

/**
 * @brief crevice namespace
 * 
 */
namespace crevice {

/**
 * @brief Render Graph class
 * This implements the render graph in vulkan
 * 
 */
class RenderGraph {
 private:
  uint32_t rid;
  uint32_t mPassTotal;
  uint32_t mAttachTotal;

  uint8_t MAX_FRAMES_IN_FLIGHT = 2;

  bool mReady = false;

  // Graph data
  HashMap<uint32_t, SharedPtr<RenderPass>> renderPasses;
  VectorMap<uint32_t, VectorSet<uint32_t>> dependencyEdges;
  HashSet<uint32_t> outputNodes;

  VectorMap<uint32_t, RGAttachment> attachments;
  VectorSet<uint32_t> attsUsing;
  VectorMap<uint32_t, uint32_t> attIndexMap;
  VectorSet<uint32_t> attsInput;

  // Instance data
  Vector<SharedPtr<VkRenderPass>> renderPassInsts;
  // get attachment order from id after compile
  // VectorMap<uint32_t, uint32_t> mAttachmentMap;
 public:
  Deque<uint32_t> mExeOrder;
  HashSet<uint32_t> startNodes;

 private:
  VectorSet<uint32_t> orphanNodes;

  // TOOD better way to manage resource;
  GpuResourceManager* mGpuRManager;

  VectorMap<uint32_t, FrameResource<VkImageView>> externalImageViews;
  VectorMap<uint32_t, FrameResource<CVTexture>> internalImages;

  Vector<FrameResource<VkFramebuffer>> mFramebuffers;
  Vector<FrameResource<VkCommandBuffer>> mCommandBuffers;

  Vector<VkSemaphore> imageAvailableSemaphores;
  Vector<VkSemaphore> renderFinishedSemaphores;
  size_t currentFrame = 0;
  Vector<VkFence> imagesInFlight;
  Vector<VkFence> inFlightFences;

  // TODO
  bool framebufferResized = false;

 public:
  /**
  * @brief Set the Gpu Resource Manager object
  * 
  * @param man 
  */
  void setGpuRManager(GpuResourceManager* man) { mGpuRManager = man; };

  /**
   * @brief add a renderPass
   * 
   * @param pass 
   * @return uint32_t 
   */
  uint32_t addPass(SharedPtr<RenderPass> pass);

  /**
   * @brief remove a renderPass
   * 
   * @param passId 
   */
  void removePass(uint32_t passId);

  /**
   * @brief Get the Pass object
   * 
   * @param passId 
   * @return SharedPtr<RenderPass> 
   */
  SharedPtr<RenderPass> getPass(uint32_t passId);

  /**
   * @brief Set the External Image View object
   * 
   * @param attachId 
   * @param view 
   */
  void setExternalImageView(uint32_t attachId,
                            FrameResource<VkImageView> view) {
    externalImageViews[attachId] = view;
  }

  /**
   * @brief Create a Render Pass Instance With Sub Pass object
   * 
   */
  void createRenderPassInstanceWithSubPass();

  /**
   * @brief Set the Output Pass object
   * 
   * @param passId 
   */
  void setOutputPass(uint32_t passId) { outputNodes.insert(passId); }

  /**
   * @brief remove Output Pass
   * 
   * @param passId 
   */
  void removeOutputPass(uint32_t passId) { outputNodes.erase(passId); }

  /**
   * @brief add a attachment
   * 
   * @param attachment 
   * @return uint32_t 
   */
  uint32_t addAttachment(RGAttachment attachment);

  /**
   * @brief remove a attachment
   * 
   * @param attachId 
   */
  void removeAttachment(uint32_t attachId);

  /**
   * @brief link two Pass Node
   * 
   * @param passId1 
   * @param passId2 
   */
  void linkNode(uint32_t passId1, uint32_t passId2);

  /**
   * @brief unlink two Pass Node
   * 
   * @param passId1 
   * @param passId2 
   */
  void detachNode(uint32_t passId1, uint32_t passId2);


  /**
   * @brief compile render graph to vulkan SubPass
   * 
   */
  void compileWithSubPass();

  /**
   * @brief Create a Sync Obj object
   * 
   */
  void createSyncObj();

  /**
   * @brief Create a Command Buffer object
   * 
   */
  void createCommandBuffer();

  /**
   * @brief 
   * 
   */
  void analyzeExecutionOrder();

  /**
   * @brief Create a Frame Buffer For Sub Pass object
   * 
   */
  void createFrameBufferForSubPass();

  /**
   * @brief Create a Internal Image Views object
   * 
   */
  void createInternalImageViews();

  // void setRenderCmd();

  // bool isReady() { return mReady; }

  // void bind();

  /**
   * @brief update RenderData
   * 
   * @param perpassList 
   * @param renderList 
   */
  void updateRenderData(Vector<PerPassRenderAble> perpassList,
                        Vector<RenderAble> renderList);

  /**
   * @brief record Frame With Subpass
   * 
   * @param frame 
   * @param imageIndex 
   */
  void recordFrameWithSubpass(uint64_t frame, uint32_t& imageIndex);

  /**
   * @brief 
   * 
   * @param frame 
   * @param imageIndex 
   */
  void submit(uint64_t frame, uint32_t& imageIndex);

  /**
   * @brief 
   * 
   * @param frame 
   * @param imageIndex 
   */
  void present(uint64_t frame, uint32_t& imageIndex);

  /**
   * @brief 
   * 
   * @param frame 
   */
  void drawFrame(uint64_t frame);

  /**
   * @brief Get the Descriptor Set Layouts object
   * 
   * @param passId 
   * @return Vector<SharedPtr<VkDescriptorSetLayout>> 
   */
  Vector<SharedPtr<VkDescriptorSetLayout>> getDescriptorSetLayouts(
      uint32_t passId) {
    return renderPasses[passId]->getDescriptorSetLayouts();
  };

  /**
   * @brief 
   * 
   */
  void clear();

  /**
   * @brief Construct a new Render Graph object
   * 
   */
  RenderGraph() {
    mAttachTotal = 0;
    mPassTotal = 0;
  }

  /**
   * @brief Destroy the Render Graph object
   * 
   */
  ~RenderGraph() {}
};

}  // namespace crevice
