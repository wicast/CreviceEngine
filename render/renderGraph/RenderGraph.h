#ifndef CREVICE_RG
#define CREVICE_RG 1

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

namespace crevice {

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
  void setGpuRManager(GpuResourceManager* man) { mGpuRManager = man; };

  uint32_t addPass(SharedPtr<RenderPass> pass);
  void removePass(uint32_t passId);
  SharedPtr<RenderPass> getPass(uint32_t passId);
  void setExternalImageView(uint32_t attachId,
                            FrameResource<VkImageView> view) {
    externalImageViews[attachId] = view;
  }
  void createRenderPassInstanceWithSubPass();
  void setOutputPass(uint32_t passId) { outputNodes.insert(passId); }
  void removeOutputPass(uint32_t passId) { outputNodes.erase(passId); }

  uint32_t addAttachment(RGAttachment attachment);
  void removeAttachment(uint32_t attachId);

  void linkNode(uint32_t passId1, uint32_t passId2);
  void detachNode(uint32_t passId1, uint32_t passId2);

  void compileWithSubPass();
  void createSyncObj();
  void createCommandBuffer();
  void analyzeExecutionOrder();
  void createFrameBufferForSubPass();
  void createInternalImageViews();

  // void setRenderCmd();

  bool isReady() { return mReady; }

  void bind();

  void updateRenderData(Vector<PerPassRenderAble> perpassList,
                        Vector<RenderAble> renderList);

  void recordFrameWithSubpass(uint64_t frame);
  void submit(uint64_t frame, uint32_t& imageIndex);
  void present(uint64_t frame, uint32_t& imageIndex);
  void drawFrame(uint64_t frame);


  Vector<SharedPtr<VkDescriptorSetLayout>> getDescriptorSetLayouts(uint32_t passId){
    return renderPasses[passId]->getDescriptorSetLayouts();
  };         

  void clear();

  RenderGraph() {
    mAttachTotal = 0;
    mPassTotal = 0;
  }
  ~RenderGraph() {}
};

}  // namespace crevice

#endif