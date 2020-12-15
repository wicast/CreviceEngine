/**
 * @file RenderPass.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "volk_imp.h"
#include <tuple>

#include "GpuResourceManager.h"
#include "descriptor/ShaderInputKey.h"
#include "stl/CreviceSTL.h"
#include "FrameResource.h"
#include "descriptor/VertexInputInfo.h"
#include "RenderAble.h"

namespace crevice {

// struct shaderInputTypes {};

/**
 * @brief Render pass object
 * 
 */
class RenderPass {
 public:
  /**
   * @brief attachment ids for input
   * 
   */
  VectorSet<uint32_t> inputAttachments;

  /**
   * @brief attachment ids for output
   * 
   */
  VectorSet<uint32_t> outputAttachments;
  // uint32_t attachmentsDependency;

  /**
   * @brief 
   * 
   */
  VertexInputInfo vertexInputInfoDesc;

  // From camera and certain pass or bake
  // int perPassBufferDes;//ViewProjection,etc
  // int perPassTextureDes;//shadowMap,depthBuffer,etc. This may related to slot
  // in renderPass
  // pair by render pass assign component
  
  /**
   * @brief perpass input value keys
   * 
   */
  // TODO same key can only appear once, so better use set
  Vector<ShaderInputKey> perPassInputKeys;
  
  /**
   * @brief Used for render to texture
   * 
   */
  VectorSet<uint32_t> innerImage;

  // int perObjLayout;
  /**
   * @brief this is from Material and Transform
   * 
   */
  Vector<ShaderInputKey> perObjInputKeys;
  // TODO
  uint32_t materialFamily;

  /**
   * @brief shaders
   * 
   */
  std::tuple<String, String> shaderAsset;

  // TODO now only impl a fixed drawMethod
  // int drawMethod;

  bool enabled = true;
  uint32_t id;
  String mName;
  HashSet<String> tags;

  // instance data
  /**
   * @brief VkDescriptorSetLayouts
   * 
   */
  Vector<SharedPtr<VkDescriptorSetLayout>> mSetLayouts;

  /**
   * @brief vulkan render pass
   * 
   */
  SharedPtr<VkRenderPass> mRenderPass;

  /**
   * @brief vulkan pipeline
   * 
   */
  SharedPtr<VkPipeline> mPipeline;

  /**
   * @brief vulkan PipelineLayout
   * 
   */
  SharedPtr<VkPipelineLayout> mPipelineLayout;
  //Framebuffer index from render graph

  // TODO this must be frame resource
  PerPassRenderAble mPerpassRenderable;
  Vector<RenderAble> mRenderList;

  /**
   * @brief change pass name
   * 
   * @param name 
   */
  void changeName(String name) { mName = name; }

  /**
   * @brief add a tag
   * 
   * @param tag 
   */
  void addTag(String tag) { tags.emplace(tag); }

  /**
   * @brief remove a Tag
   * 
   * @param tag 
   */
  void removeTag(String tag) { tags.erase(tag); }

  /**
   * @brief change Shader
   * 
   * @param vert 
   * @param frag 
   */
  void changeShader(String vert, String frag) { shaderAsset = {vert, frag}; }

  /**
   * @brief generateVertexInputInfo 
   * 
   * @return std::tuple<VkVertexInputBindingDescription, Vector<VkVertexInputAttributeDescription>> 
   */
 std::tuple<VkVertexInputBindingDescription, Vector<VkVertexInputAttributeDescription>> generateVertexInputInfo();
  void generateDescriptorSetLayout(GpuResourceManager& gManager);
  Vector<SharedPtr<VkDescriptorSetLayout>> getDescriptorSetLayouts(){
    return mSetLayouts;
  };

  /**
   * @brief genreatePipeline
   * 
   * @param gManager 
   */
  void genreatePipeline(GpuResourceManager& gManager);
  
  /**
   * @brief compile single pass
   * 
   * @param gManager 
   * @param vkRenderPass 
   */
  void compile(GpuResourceManager& gManager,
              //  VectorMap<uint32_t, uint32_t> attachmentMap,
               SharedPtr<VkRenderPass> vkRenderPass);


  /**
   * @brief add Perpass RenderAble
   * 
   * @param pa 
   */
  void addPerpassRenderAble(PerPassRenderAble pa) {mPerpassRenderable = pa;};

  /**
   * @brief 
   * 
   * @param ra 
   */
  void addRenderAble(RenderAble ra) {mRenderList.push_back(ra);};

  /**
   * @brief 
   * 
   * @param frame 
   */
  void updateRenderList(uint64_t frame);

  /**
   * @brief 
   * 
   * @param frame 
   * @param commandBuffer 
   */
  void recordFrameWithSubpass(uint64_t frame,VkCommandBuffer commandBuffer);

  /**
   * @brief 
   * 
   */
  RenderPass(/* args */) {}

  /**
   * @brief Destroy the Render Pass object
   * 
   */
  ~RenderPass() {}
};

}  // namespace crevice
