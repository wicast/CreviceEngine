#ifndef CREVICE_RG
#define CREVICE_RG 1

#include "render/FrameResource.h"
#include "render/renderGraph/RGAttachment.h"
#include "render/renderGraph/RenderPass.h"
#include "stl/CreviceHashMap.h"
#include "stl/CreviceHashSet.h"
#include "stl/CreviceSharedPtr.h"
#include "stl/CreviceVector.h"
#include "stl/CreviceVectorMap.h"
#include "stl/CreviceDeque.h"

namespace crevice {

class RenderGraph {
 private:
  uint32_t rid;
  uint32_t mPassTotal;
  uint32_t mAttachTotal;

  bool mReady = false;

  // Graph data
  HashMap<uint32_t, SharedPtr<RenderPass>> renderPasses;
  VectorMap<uint32_t,VectorSet<uint32_t>> dependencyEdges;
  HashSet<uint32_t> outputNodes;

  VectorMap<uint32_t, RGAttachment> attachments;

  // Instance data

  // get attachment order from id after compile
  // VectorMap<uint32_t, uint32_t> mAttachmentMap;
  Deque<uint32_t> mExeOrder;
  VectorSet<uint32_t> orphanNodes;
  
  Vector<FrameResource<VkFramebuffer>> mFramebuffers;
  Vector<FrameResource<VkCommandBuffer>> mCommandBuffers;

  void analyzeExecutionOrder();
 public:
  uint32_t addPass(SharedPtr<RenderPass> pass);
  void removePass(uint32_t passId);
  SharedPtr<RenderPass> getPass(uint32_t passId);
  void setOutputPass(uint32_t passId) {
    outputNodes.insert(passId);
  }
  void removeOutputPass(uint32_t passId) {
    outputNodes.erase(passId);
  }

  uint32_t addAttachment(RGAttachment attachment);
  void removeAttachment(uint32_t attachId);

  void linkNode(uint32_t passId1, uint32_t passId2);
  void detachNode(uint32_t passId1, uint32_t passId2);

  void compile();

  auto getExeOrder() {
    return mExeOrder;
  }

  // void setRenderCmd();

  bool isReady() { return mReady; }

  void bind();

  void updateRenderData();

  void drawFrame();

  void clear();

  RenderGraph() {
  mAttachTotal = 0;
  mPassTotal = 0;
}
  ~RenderGraph() {}
};

}  // namespace crevice

#endif