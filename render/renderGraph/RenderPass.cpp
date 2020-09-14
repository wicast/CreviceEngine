#include "render/renderGraph/RenderPass.h"

namespace crevice {
void RenderPass::compile(GpuResourceManager gManager,
                         vectorMap<uint32_t, uint32_t> attachmentMap,SharedPtr<VkRenderPass> vkRenderPass) {
  // generate Layouts
  Vector<VkDescriptorSetLayoutBinding> bufferBindings;
  Vector<VkDescriptorSetLayoutBinding> perPassTexBindings;
  Vector<VkDescriptorSetLayoutBinding> perObjTexBindings;


  for (auto inKey : perPassInputKeys) {
    // TODO help generate shader header
    Vector<ShaderSlotType> bufferKeys;
    for (auto key : inKey.keys) {
      if (key == crevice::ShaderSlotType::TextureSample) {
        VkDescriptorSetLayoutBinding perTexBinding = {};
        perTexBinding.descriptorCount = perPassTexBindings.size();
        // TODO
        perTexBinding.descriptorCount = 1;
        perTexBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        perTexBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        perObjTexBindings.push_back(perTexBinding);
      } else if (key == crevice::ShaderSlotType::PixelLocalTextureSampler) {
      } else {
        bufferKeys.push_back(key);
      }
    }

    VkDescriptorSetLayoutBinding bufferBinding = {};
    bufferBinding.binding = bufferBindings.size();
    bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // TODO
    bufferBinding.descriptorCount = 1;
    bufferBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bufferBinding.pImmutableSamplers = nullptr;  // Optional
  }

  for (auto inKey : perObjInputKeys) {
    Vector<ShaderSlotType> bufferKeys;
    for (auto key : inKey.keys) {
      if (key == crevice::ShaderSlotType::TextureSample) {
        VkDescriptorSetLayoutBinding perTexBinding = {};
        perTexBinding.descriptorCount = perObjTexBindings.size();
        // TODO
        perTexBinding.descriptorCount = 1;
        perTexBinding.descriptorType =
            VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        perTexBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        perObjTexBindings.push_back(perTexBinding);
      } else if (key == crevice::ShaderSlotType::PixelLocalTextureSampler) {
      } else {
        bufferKeys.push_back(key);
      }
    }

    VkDescriptorSetLayoutBinding bufferBinding = {};
    bufferBinding.binding = bufferBindings.size();
    bufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // TODO
    bufferBinding.descriptorCount = 1;
    bufferBinding.stageFlags =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    bufferBinding.pImmutableSamplers = nullptr;  // Optional
  }

  VkDescriptorSetLayoutCreateInfo bufferLayoutInfo{};
  bufferLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  bufferLayoutInfo.bindingCount = static_cast<uint32_t>(bufferBindings.size());
  bufferLayoutInfo.pBindings = bufferBindings.data();
  //TODO reference
  gManager.addDescriptorSetLayout(bufferLayoutInfo);

  VkDescriptorSetLayoutCreateInfo perPassTexLayoutInfo{};
  perPassTexLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  perPassTexLayoutInfo.bindingCount =
      static_cast<uint32_t>(perPassTexBindings.size());
  perPassTexLayoutInfo.pBindings = perPassTexBindings.data();
  gManager.addDescriptorSetLayout(perPassTexLayoutInfo);

  VkDescriptorSetLayoutCreateInfo perObjTexLayoutInfo{};
  perObjTexLayoutInfo.sType =
      VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  perObjTexLayoutInfo.bindingCount =
      static_cast<uint32_t>(perObjTexBindings.size());
  perObjTexLayoutInfo.pBindings = perObjTexBindings.data();
  gManager.addDescriptorSetLayout(perObjTexLayoutInfo);
}

}  // namespace crevice
