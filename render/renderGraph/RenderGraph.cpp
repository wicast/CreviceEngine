#include "render/renderGraph/RenderGraph.h"

namespace crevice {

uint32_t RenderGraph::addPass(SharedPtr<RenderPass> pass) {
  passTotal++;
  renderPasses.emplace(passTotal, pass);

  beginNodes.emplace(passTotal);
  return passTotal;
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
}

SharedPtr<RenderPass> RenderGraph::getPass(uint32_t passId) {
  return renderPasses[passId];
}

uint32_t RenderGraph::addAttachment(RGAttachment attachment) {
  attachTotal++;
  attachment.id = attachTotal;
  attachments.emplace(attachTotal, attachment);
  return attachTotal;
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
  attachTotal = 0;
  passTotal = 0;
}
RenderGraph::~RenderGraph() {}

}  // namespace crevice