#ifndef CREVICE_RG_RENDER_PASS_H
#define CREVICE_RG_RENDER_PASS_H 1

#include <volk.h>

#include "render/descriptor/shaderInputKey.h"
#include "stl/CreviceSTL.h"
#include "render/GpuResourceManager.h"

namespace crevice {

struct shaderInputTypes {};

class RenderPass {
 private:
  // Attachments
  // attachment ids
  VectorSet<uint32_t> attachmentInput;

  VectorSet<uint32_t> attachmentsOutput;
  // uint32_t attachmentsDependency;

  // TODO
  int vertexInputInfo;

  // From camera and certain pass or bake
  // int perPassBufferDes;//ViewProjection,etc
  // int perPassTextureDes;//shadowMap,depthBuffer,etc. This may related to slot
  // in renderPass
  // pair by render pass assign component
  //TODO same key can only appear once
  Vector<ShaderInputKey> perPassInputKeys;

  // from Material and Transform

  // int perObjLayout;
  Vector<ShaderInputKey> perObjInputKeys;
  // TODO
  uint32_t materialFamily;

  int shaderRes;

  // TODO now only impl a fixed drawMethod
  // int drawMethod;

  bool enabled = true;
  // uint32_t id;
  String name;
  HashSet<String> tags;

  // instance data
  HashSet<SharedPtr<VkDescriptorSetLayout>> setLayout;
  SharedPtr<VkRenderPass> vkRenderPass;
  SharedPtr<VkPipeline> vkPipeline;
  SharedPtr<VkPipelineLayout> vkPipelineLayout;
  SharedPtr<VkFramebuffer> vkFramebuffer;

  // TODO
  Vector<bool> renderList;

 public:
  void changeName(String _name) { name = _name; }

  void addTag(String tag) { tags.emplace(tag); }

  void removeTag(String tag) { tags.erase(tag); }

  void changeShader(int s) { shaderRes = s; }

  void compile(GpuResourceManager gManager,vectorMap<uint32_t, uint32_t> attachmentMap,SharedPtr<VkRenderPass> vkRenderPass);

  RenderPass(/* args */);
  ~RenderPass();
};

}  // namespace crevice

#endif