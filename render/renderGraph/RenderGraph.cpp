#include "render/renderGraph/RenderGraph.h"

namespace crevice {

uint32_t RenderGraph::addPass(SharedPtr<RenderPass> pass) {
  mPassTotal++;
  renderPasses[mPassTotal] = pass;

  pass->id = mPassTotal;
  return mPassTotal;
}

void RenderGraph::removePass(uint32_t passId) {
  // remove edges
  dependencyEdges.erase(passId);
  // TODO more efficient edge remove
  for (auto edge : dependencyEdges) {
    for (auto edgeId : edge.second) {
      if (passId == edgeId) {
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
  // TODO for now only support single renderpass with subpass
  analyzeExecutionOrder();
  // analyze attachments and create renderpass
  createRenderPassInstanceWithSubPass();
  // create framebuffer
  createFrameBufferForSubPass();
  // TODO create commandBuffer
}

void RenderGraph::createRenderPassInstanceWithSubPass() {
  for (auto passId : mExeOrder) {
    auto p = *(renderPasses[passId]);
    attsUsing.insert(p.inputAttachments.begin(), p.inputAttachments.end());
    attsUsing.insert(p.outputAttachments.begin(), p.outputAttachments.end());

    attsInput.insert(p.inputAttachments.begin(), p.inputAttachments.end());
  }

  Vector<VkAttachmentDescription> attachmentsInst;
  uint32_t inx = 0;
  for (auto attUsingId : attsUsing) {
    auto rgAttach = attachments[attUsingId];
    VkAttachmentDescription attDes;
    attDes.format = rgAttach.format;
    attDes.samples = VK_SAMPLE_COUNT_1_BIT;

    attDes.loadOp = rgAttach.loadOp;
    attDes.storeOp = rgAttach.storeOp;
    attDes.stencilLoadOp = rgAttach.stencilLoadOp;
    attDes.stencilStoreOp = rgAttach.stencilStoreOp;

    attDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (rgAttach.type == RGAttachmentTypes::Color) {
      attDes.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    } else if (rgAttach.type == RGAttachmentTypes::DepthStencil) {
      attDes.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    } else if (rgAttach.type == RGAttachmentTypes::Present) {
      attDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    attachmentsInst.push_back(attDes);
    attIndexMap[attUsingId] = inx;
    inx++;
  }

  // Generate subpass
  Vector<VkSubpassDescription> subpassDescriptions;

  VectorMap<uint32_t, uint32_t> passIdxMap;
  inx = 0;
  for (auto passId : mExeOrder) {
    auto p = *(renderPasses[passId]);

    Vector<VkAttachmentReference> colorReference;
    Vector<VkAttachmentReference> depthReference;
    Vector<VkAttachmentReference> inputReferences;
    for (auto col : p.inputAttachments) {
      auto idx = attIndexMap[col];
      colorReference.push_back({idx, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
    }

    for (auto col : p.outputAttachments) {
      auto idx = attIndexMap[col];
      if (attachments[idx].type == RGAttachmentTypes::Color) {
        colorReference.push_back({idx, attachmentsInst[idx].finalLayout});
      } else if (attachments[idx].type == RGAttachmentTypes::DepthStencil) {
        depthReference.push_back({idx, attachmentsInst[idx].finalLayout});
      }
    }

    VkSubpassDescription subpassDes;
    subpassDes.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDes.colorAttachmentCount = colorReference.size();
    subpassDes.pColorAttachments = colorReference.data();
    subpassDes.pDepthStencilAttachment = depthReference.data();
    subpassDes.inputAttachmentCount = inputReferences.size();
    subpassDes.pInputAttachments = inputReferences.data();

    subpassDescriptions.push_back(subpassDes);

    passIdxMap[passId] = inx;
    inx++;
  }

  Vector<VkSubpassDependency> dependencies;

  for (auto passId : startNodes) {
    VkSubpassDependency vkdep;
    vkdep.srcSubpass = VK_SUBPASS_EXTERNAL;
    vkdep.dstSubpass = passIdxMap[passId];
    vkdep.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    vkdep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    vkdep.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    vkdep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                          VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    vkdep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies.push_back(vkdep);
  }

  for (auto deps : dependencyEdges) {
    for (auto src : deps.second) {
      VkSubpassDependency vkdep;
      vkdep.srcSubpass = passIdxMap[src];
      vkdep.dstSubpass = passIdxMap[deps.first];
      vkdep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
      vkdep.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
      vkdep.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
      vkdep.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
      vkdep.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
      dependencies.push_back(vkdep);
    }
  }

  for (auto endPassId : outputNodes) {
    dependencies[2].srcSubpass = passIdxMap[endPassId];
    dependencies[2].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[2].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[2].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[2].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[2].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
  }

  VkRenderPassCreateInfo renderPassInfoCI{};
  renderPassInfoCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfoCI.attachmentCount =
      static_cast<uint32_t>(attachmentsInst.size());
  renderPassInfoCI.pAttachments = attachmentsInst.data();
  renderPassInfoCI.subpassCount =
      static_cast<uint32_t>(subpassDescriptions.size());
  renderPassInfoCI.pSubpasses = subpassDescriptions.data();
  renderPassInfoCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
  renderPassInfoCI.pDependencies = dependencies.data();

  auto renderPassIns = mGpuRManager->createRenderPass(renderPassInfoCI);

  renderPassInsts.push_back(renderPassIns);
}

void RenderGraph::analyzeExecutionOrder() {
  // Topological sorting
  // https://www.wikiwand.com/en/Topological_sorting#/Kahn's_algorithm

  if (outputNodes.empty()) {
    throw std::runtime_error("no output node detected");
  }

  // collect node out degrees;
  HashMap<uint32_t, uint32_t> _nodeOutDegree;
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
        _originOrphanNodes.insert(depNode);
      }
    }
  }

  // collect input nodes
  HashMap<uint32_t, uint32_t> _inDegree;
  for (auto node : renderPasses) {
    if (dependencyEdges.count(node.first) == 0) {
      startNodes.insert(node.first);
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
    mExeOrder.push_front(aNode);
    for (auto depNode : dependencyEdges[aNode]) {
      _nodeOutDegree[depNode]--;
      if (_nodeOutDegree[depNode] == 0) {
        _exeOrderQueueS.insert(depNode);
      }
    }
  }

  for (auto degInfo : _nodeOutDegree) {
    if (degInfo.second != 0) {
      throw std::runtime_error("cycle detected!");
    }
  }
}

void RenderGraph::createFrameBufferForSubPass() {
  createInternalImageViews();

  auto width = mGpuRManager->vkContext->windowContext->swapChainExtent.width;
  auto height = mGpuRManager->vkContext->windowContext->swapChainExtent.height;

  Vector<VkFramebufferCreateInfo> frameBufferCIs{};

  auto swapSize = GpuResourceManager::swapChainSize;
  for (size_t i = 0; i < swapSize; i++) {
    Vector<VkImageView> views;
    for (auto attId : attsUsing) {
      if (externalImageViews.count(attId) != 0) {
        views.push_back(*(externalImageViews[attId].getForUpdate(i)));
      } else if (internalImages.count(attId) != 0) {
        views.push_back(
            internalImages[attId].getForUpdate(i)->textureImageView);
      }
    }

    VkFramebufferCreateInfo frameBufferCI{};
    frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCI.renderPass = *(renderPassInsts[0]);
    frameBufferCI.attachmentCount = views.size();
    frameBufferCI.pAttachments = views.data();
    frameBufferCI.width = width;
    frameBufferCI.height = height;
    frameBufferCI.layers = 1;

    frameBufferCIs.push_back(frameBufferCI);
  }

  mFramebuffers[0] =
      mGpuRManager->createFrameResourceFramebuffer(frameBufferCIs);
}

void RenderGraph::createInternalImageViews() {
  VectorSet<uint32_t> attsOut;
  eastl::set_difference(attsUsing.begin(), attsUsing.end(), attsInput.begin(),
                        attsInput.end(),
                        eastl::inserter(attsOut, attsOut.begin()));

  for (auto usingId : attsUsing) {
    auto attach = attachments[usingId];
    if (externalImageViews.count(usingId) == 0) {
      VkImageUsageFlags usage;
      if (attsOut.count(usingId) != 0 &&
          attach.type == RGAttachmentTypes::Color) {
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
      } else if (attsOut.count(usingId) != 0 &&
                 attach.type == RGAttachmentTypes::DepthStencil) {
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
      }
      if (attsInput.count(usingId) != 0) {
        usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
      }

      auto width =
          mGpuRManager->vkContext->windowContext->swapChainExtent.width;
      auto height =
          mGpuRManager->vkContext->windowContext->swapChainExtent.height;
      internalImages[usingId] = mGpuRManager->createFrameResourceEmptyTexture(
          width, height, usage, GpuResourceManager::swapChainSize);
    }
  }

  internalImages;
}

void RenderGraph::bind() {}

void RenderGraph::updateRenderData() {}

void RenderGraph::drawFrame() {}

}  // namespace crevice