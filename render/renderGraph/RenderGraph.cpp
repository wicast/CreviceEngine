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

void RenderGraph::compileWithSubPass() {
  //create sync obj
  createSyncObj();
  // TODO for now only support single renderpass with subpass
  analyzeExecutionOrder();
  // analyze attachments and create renderpass
  createRenderPassInstanceWithSubPass();
  // create framebuffer
  createFrameBufferForSubPass();
  // create commandBuffer
  createCommandBuffer();
  // compile sub passes
  for (auto passId : mExeOrder) {
    auto pass = renderPasses[passId];
    pass->compile(*mGpuRManager, renderPassInsts[0]);
  }
}

void RenderGraph::createSyncObj() {
  auto vkContext = mGpuRManager->vkContext;

  imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
  inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
  imagesInFlight.resize(mGpuRManager->swapChainSize, VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    if (vkCreateSemaphore(vkContext->device, &semaphoreInfo, nullptr,
                          &imageAvailableSemaphores[i]) != VK_SUCCESS ||
        vkCreateSemaphore(vkContext->device, &semaphoreInfo, nullptr,
                          &renderFinishedSemaphores[i]) != VK_SUCCESS ||
        vkCreateFence(vkContext->device, &fenceInfo, nullptr,
                      &inFlightFences[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create semaphores for a frame!");
    }
  }
}

void RenderGraph::createCommandBuffer() {
  // TODO different renderpass with different commandbuffer
  mCommandBuffers.push_back(
      mGpuRManager->createCommandBuffers(GpuResourceManager::swapChainSize));
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

void RenderGraph::updateRenderData(Vector<PerPassRenderAble> perpassList,
                                   Vector<RenderAble> renderList) {
  for (auto perRenderable : perpassList) {
    renderPasses[perRenderable.passId]->addPerpassRenderAble(perRenderable);
  }
  for (auto renderable : renderList) {
    renderPasses[renderable.passId]->addRenderAble(renderable);
  }
}

void RenderGraph::drawFrameWithSubpass(uint64_t frame) {
  // First:record commands
  // reset all current commands
  for (auto cb : mCommandBuffers) {
    vkResetCommandBuffer(*(cb.getForUpdate(frame)),
                         VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
  }

  // pass the commanduffer
  // begin renderpass
  auto commandBuffer = *(mCommandBuffers[0].getForUpdate(frame));

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;                   // Optional
  beginInfo.pInheritanceInfo = nullptr;  // Optional

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = *renderPassInsts[0];
  renderPassInfo.framebuffer = *(mFramebuffers[0].getForUpdate(frame));
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent =
      mGpuRManager->vkContext->windowContext->swapChainExtent;

  // TODO clear color setup
  Vector<VkClearValue> clearValues = {};
  for (auto _ : mExeOrder) {
    VkClearValue clearValue;
    clearValue.color = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValue.depthStencil = {1.0f, 0};
    clearValues.push_back(clearValue);
  }

  renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
  renderPassInfo.pClearValues = clearValues.data();

  vkCmdBeginRenderPass(commandBuffer, &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  // iterate subpass
  uint32_t count = 1;
  for (auto passId : mExeOrder) {
    auto pass = renderPasses[passId];
    pass->drawFrameWithSubpass(frame, commandBuffer);
    if (count != mExeOrder.size()) {
      vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    }
    count++;
  }

  vkCmdEndRenderPass(commandBuffer);
  // second submit

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}


// vulkan can submit multiple commandBuffers
void RenderGraph::submit(uint64_t frame, uint32_t& imageIndex) {
  auto vkContext = mGpuRManager->vkContext;
  auto windowContext = vkContext->windowContext;

  vkWaitForFences(vkContext->device, 1, &inFlightFences[currentFrame], VK_TRUE,
                  UINT64_MAX);

  VkResult vkResult = vkAcquireNextImageKHR(
      vkContext->device, windowContext->swapChain, UINT64_MAX,
      imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
  if (vkResult == VK_ERROR_OUT_OF_DATE_KHR) {
    // TODO
    // recreateSwapChain();
    return;
  } else if (vkResult != VK_SUCCESS && vkResult != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
    vkWaitForFences(vkContext->device, 1, &imagesInFlight[imageIndex], VK_TRUE,
                    UINT64_MAX);
  }
  imagesInFlight[imageIndex] = inFlightFences[currentFrame];

  // updateUniformBuffer(imageIndex);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
  VkPipelineStageFlags waitStages[] = {
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  // TODO get all commandBuffer
  auto commandBuff = *(mCommandBuffers[0].getForUpdate(frame));
  submitInfo.pCommandBuffers = &commandBuff;

  VkSemaphore renderFinishSemaphores[] = {
      renderFinishedSemaphores[currentFrame]};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = renderFinishSemaphores;

  vkResetFences(vkContext->device, 1, &inFlightFences[currentFrame]);
  if (vkQueueSubmit(vkContext->graphicsQueue, 1, &submitInfo,
                    inFlightFences[currentFrame]) != VK_SUCCESS) {
    throw std::runtime_error("failed to submit draw command buffer!");
  }
}

void RenderGraph::present(uint64_t frame, uint32_t& imageIndex,
                          VkSemaphore renderFinishSemaphores[]) {
  auto vkContext = mGpuRManager->vkContext;
  auto windowContext = vkContext->windowContext;

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = renderFinishSemaphores;

  VkSwapchainKHR swapChains[] = {windowContext->swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;

  presentInfo.pResults = nullptr;  // Optional
  VkResult vkResult = vkQueuePresentKHR(vkContext->presentQueue, &presentInfo);
  if (vkResult == VK_ERROR_OUT_OF_DATE_KHR || vkResult == VK_SUBOPTIMAL_KHR ||
      framebufferResized) {
    framebufferResized = false;
    // TODO
    // recreateSwapChain();
  } else if (vkResult != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image!");
  }

  currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

}  // namespace crevice