#include "render/renderGraph/RenderGraph.h"

namespace crevice {

uint32_t RenderGraph::addPass(SharedPtr<RenderPass> pass) {
  mPassTotal++;
  renderPasses.emplace(mPassTotal, pass);

  beginNodes.emplace(mPassTotal);
  return mPassTotal;
}

void RenderGraph::removePass(uint32_t passId) {
  // remove edges
  // TODO more efficient edge remove
  for (auto edge : edges) {
    if (edge.first == passId || edge.next == passId) {
      edges.erase(edge);
    }
  }
  //remove pass
  renderPasses.erase(passId);

  if (beginNodes.count(passId) != 0 )
  {
    beginNodes.erase(passId);
  }
}

SharedPtr<RenderPass> RenderGraph::getPass(uint32_t passId) {
  return renderPasses[passId];
}

uint32_t RenderGraph::addAttachment(RGAttachment attachment) {
  mAttachTotal++;
  attachment.id = mAttachTotal;
  attachments.emplace(mAttachTotal, attachment);
  return mAttachTotal;
}

void RenderGraph::removeAttachment(uint32_t attachId) {
  attachments.erase(attachId);
}

void RenderGraph::linkNode(uint32_t passId1, uint32_t passId2) {
  Dependency dep = {passId1, passId2};
  edges.emplace(dep);

  if (beginNodes.count(passId2) != 0) {
    beginNodes.erase(passId2);
  }
}

void RenderGraph::detachNode(uint32_t passId1, uint32_t passId2) {
  Dependency dep = {passId1, passId2};
  edges.erase(dep);

  if (beginNodes.count(passId2) == 0) {
    beginNodes.emplace(passId2);
  }
}

void RenderGraph::compile() {}

bool RenderGraph::isReady() {return false;}

void RenderGraph::bind() {}

void RenderGraph::updateRenderData() {}

void RenderGraph::drawFrame() {}

RenderGraph::RenderGraph(/* args */) {
  mAttachTotal = 0;
  mPassTotal = 0;
}
RenderGraph::~RenderGraph() {}

}  // namespace crevice