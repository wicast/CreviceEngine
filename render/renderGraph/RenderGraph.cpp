#include "render/renderGraph/RenderGraph.h"

namespace crevice {

uint32_t RenderGraph::addPass(SharedPtr<RenderPass> pass) {
  mPassTotal++;
  renderPasses[mPassTotal] = pass;

  outputNodes.insert(mPassTotal);
  return mPassTotal;
}

void RenderGraph::removePass(uint32_t passId) {
  // remove edges
  dependencyEdges.erase(passId);
  // TODO more efficient edge remove
  for (auto edge : dependencyEdges) {
    for (auto edgeId: edge.second)
    {
      if (passId == edgeId)
      {
        dependencyEdges[edge.first].erase(passId);
      }
    }
  }
  // remove pass
  renderPasses.erase(passId);

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

void RenderGraph::linkNode(uint32_t firstPassId, uint32_t secondPassId) {
  dependencyEdges[secondPassId].insert(firstPassId);
}

void RenderGraph::detachNode(uint32_t firstPassId, uint32_t secondPassId) {
  dependencyEdges[secondPassId].erase(firstPassId);
  
}

void RenderGraph::compile() {
  analyzeExecutionOrder();
  // TODO create framebuffer
  // TODO create commandBuffer
}

void RenderGraph::analyzeExecutionOrder() {
  // Topological sorting
  // https://www.wikiwand.com/en/Topological_sorting#/Kahn's_algorithm

  if (outputNodes.empty())
  {
    std::runtime_error("no output node detected");
  }
  
  // collect node out degree;
  HashMap<uint32_t,uint32_t> _nodeOutDegree;
  for (auto node : renderPasses) {
    _nodeOutDegree[node.first] = 0;
  }
  for (auto deps : dependencyEdges) {
    for (auto nodeId : deps.second) {
      _nodeOutDegree[nodeId]++;
    }
  }
  // collect orphan nodes
  VectorSet<uint32_t> _originOrphanNodes;
  for (auto degreeInfo : _nodeOutDegree) {
    if (outputNodes.count(degreeInfo.first) == 0 && degreeInfo.second == 0) {
      _originOrphanNodes.insert(degreeInfo.first);
    }
  }

  while (!_originOrphanNodes.empty()) {
    auto aNode = _originOrphanNodes.back();
    _originOrphanNodes.pop_back();
    orphanNodes.insert(aNode);
    for (auto depNode : dependencyEdges[aNode]) {
      _nodeOutDegree[depNode]--;
      if (_nodeOutDegree[depNode] == 0) {
        _originOrphanNodes.insert(_nodeOutDegree[depNode]);
      }
    }
  }

  // do topological sorting
  VectorSet<uint32_t> _exeOrderQueueS;
  for (auto degreeInfo : _nodeOutDegree) {
    if (orphanNodes.count(degreeInfo.first) == 0 && degreeInfo.second == 0) {
      _exeOrderQueueS.insert(degreeInfo.first);
    }
  }
  while (!_exeOrderQueueS.empty()) {
    auto aNode = _exeOrderQueueS.back();
    _exeOrderQueueS.pop_back();
    mExeOrder.push_back(aNode);
    for (auto depNode : dependencyEdges[aNode]) {
      _nodeOutDegree[depNode]--;
      if (_nodeOutDegree[depNode] == 0) {
        _exeOrderQueueS.insert(_nodeOutDegree[depNode]);
      }
    }
  }

  for (auto degInfo : _nodeOutDegree) {
    if (degInfo.second != 0) {
      std::runtime_error("cycle detected!");
    }
  }
}

bool RenderGraph::isReady() { return false; }

void RenderGraph::bind() {}

void RenderGraph::updateRenderData() {}

void RenderGraph::drawFrame() {}

RenderGraph::RenderGraph(/* args */) {
  mAttachTotal = 0;
  mPassTotal = 0;
}
RenderGraph::~RenderGraph() {}

}  // namespace crevice