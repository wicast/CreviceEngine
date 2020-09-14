#ifndef CREVICE_RG
#define CREVICE_RG 1

#include "render/renderGraph/RenderPass.h"
#include "render/renderGraph/RGAttachment.h"
#include "stl/CreviceSharedPtr.h"
#include "stl/CreviceVector.h"
#include "stl/CreviceHashSet.h"
#include "stl/CreviceHashMap.h"


namespace crevice {
struct Dependency {
  /* data */
  uint32_t first;
  uint32_t next;

  bool operator==(const Dependency &other) const {
    return first == other.first && next == other.next;
  }

  // size_t operator()(crevice::Dependency const &p) const {
  //   return eastl::hash<uint32_t>()(p.first) ^ eastl::hash<uint32_t>()(p.next) << 1;
  // }
};
}
namespace eastl {
template <>
struct hash<crevice::Dependency> {
  size_t operator()(crevice::Dependency const &p) const {
    return hash<uint32_t>()(p.first) ^ hash<uint32_t>()(p.next) << 1;
  }
};
}

namespace crevice {

class RenderGraph {
 private:
  uint32_t rid;
  uint32_t passTotal;
  uint32_t attachTotal;

  // Graph data
  HashMap<uint32_t,SharedPtr<RenderPass>> renderPasses;
  HashSet<Dependency> edges;
  HashSet<uint32_t> beginNodes;

  vectorMap<uint32_t, RGAttachment> attachments;

  // Instance data

  //get attachment order from id after compile
  vectorMap<uint32_t, uint32_t> _attachmentMap;
  Vector<Vector<uint32_t>> _exeOrder;


  Vector<SharedPtr<VkFramebuffer>> framebuffers;
  Vector<VkCommandBuffer> commandBuffers;


 public:
  uint32_t addPass(SharedPtr<RenderPass> pass);
  void removePass(uint32_t passId);
  SharedPtr<RenderPass> getPass(uint32_t passId);

  uint32_t addAttachment(RGAttachment attachment);
  void removeAttachment(uint32_t attachId);

  void linkNode(uint32_t passId1,uint32_t passId2);
  void detachNode(uint32_t passId1,uint32_t passId2);

  void compile();

  // void setRenderCmd();

  bool isReady();

  void bind();

  void updateRenderData();

  void drawFrame();

  void clear();

  RenderGraph(/* args */);
  ~RenderGraph();
};

}  // namespace crevice




#endif