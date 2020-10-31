#pragma once

#include "3rd/volk/volk_imp.h"
#include <tuple>

#include "render/GpuResourceManager.h"
#include "render/descriptor/ShaderInputKey.h"
#include "stl/CreviceSTL.h"
#include "render/FrameResource.h"
#include "render/descriptor/VertexInputInfo.h"
#include "render/RenderAble.h"

namespace crevice {

struct shaderInputTypes {};

class RenderPass {
 public:
  // Attachments
  // attachment ids
  VectorSet<uint32_t> inputAttachments;

  //Attachment ids
  VectorSet<uint32_t> outputAttachments;
  // uint32_t attachmentsDependency;

  VertexInputInfo vertexInputInfoDesc;

  // From camera and certain pass or bake
  // int perPassBufferDes;//ViewProjection,etc
  // int perPassTextureDes;//shadowMap,depthBuffer,etc. This may related to slot
  // in renderPass
  // pair by render pass assign component
  // TODO same key can only appear once, so better use set
  Vector<ShaderInputKey> perPassInputKeys;
  //Used for render to texture
  VectorSet<uint32_t> innerImage;

  // from Material and Transform

  // int perObjLayout;
  Vector<ShaderInputKey> perObjInputKeys;
  // TODO
  uint32_t materialFamily;

  std::tuple<String, String> shaderAsset;

  // TODO now only impl a fixed drawMethod
  // int drawMethod;

  bool enabled = true;
  uint32_t id;
  String name;
  HashSet<String> tags;

  // instance data
  Vector<SharedPtr<VkDescriptorSetLayout>> mSetLayouts;
  SharedPtr<VkRenderPass> mRenderPass;
  SharedPtr<VkPipeline> mPipeline;
  SharedPtr<VkPipelineLayout> mPipelineLayout;
  //Framebuffer index from render graph

  // TODO this must be frame resource
  PerPassRenderAble mPerpassRenderable;
  Vector<RenderAble> mRenderList;


  void changeName(String _name) { name = _name; }

  void addTag(String tag) { tags.emplace(tag); }

  void removeTag(String tag) { tags.erase(tag); }

  void changeShader(String vert, String frag) { shaderAsset = {vert, frag}; }

 std::tuple<VkVertexInputBindingDescription, Vector<VkVertexInputAttributeDescription>> generateVertexInputInfo();
  void generateDescriptorSetLayout(GpuResourceManager& gManager);
  Vector<SharedPtr<VkDescriptorSetLayout>> getDescriptorSetLayouts(){
    return mSetLayouts;
  };
  void genreatePipeline(GpuResourceManager& gManager);
  void compile(GpuResourceManager& gManager,
              //  VectorMap<uint32_t, uint32_t> attachmentMap,
               SharedPtr<VkRenderPass> vkRenderPass);


  void addPerpassRenderAble(PerPassRenderAble pa) {mPerpassRenderable = pa;};
  void addRenderAble(RenderAble ra) {mRenderList.push_back(ra);};

  void updateRenderList(uint64_t frame);

  void recordFrameWithSubpass(uint64_t frame,VkCommandBuffer commandBuffer);

  RenderPass(/* args */) {}
  ~RenderPass() {}
};

}  // namespace crevice
