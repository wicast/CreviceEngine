#include "common/AssetResourceBridge.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void AssetResourceBridge::setup(ResourceManager* rm, AssetManager* am) {
  rManager = rm;
  aManager = am;
}

Mesh AssetResourceBridge::createMeshFromObjPath(eastl::string modelPath) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn, err;

  Mesh mesh;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        modelPath.c_str())) {
    throw std::runtime_error(warn + err);
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex = {};
      vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]};

      vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                         1.0f - attrib.texcoords[2 * index.texcoord_index + 1]};

      if (!attrib.normals.empty()) {
        vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                         attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2]};
      }

      vertex.color = {1.0f, 1.0f, 1.0f};
      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
        mesh.vertices.push_back(vertex);
      }

      mesh.indices.push_back(uniqueVertices[vertex]);
    }
  }

  return mesh;
}

crevice::Image2D AssetResourceBridge::createTex2dFromPath(eastl::string path) {
  crevice::Image2D newTex{};

  int texWidth, texHeight, texChannels;
  auto pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels,
                          STBI_rgb_alpha);
  auto imageSize = texWidth * texHeight * 4;

  // TODO mip from prebuild image, not generated
  int mipLevels = static_cast<uint32_t>(
                      std::floor(std::log2(std::max(texWidth, texHeight)))) +
                  1;

  if (!pixels) {
    throw std::runtime_error("failed to load texture image!");
  }
  newTex.mipLevels = mipLevels;
  newTex.width = texWidth;
  newTex.height = texHeight;

  newTex.rawL0.size = imageSize;
  newTex.rawL0.rawData = static_cast<unsigned char*>(pixels);

  return newTex;
}

AssetResourceBridge::AssetResourceBridge(/* args */) {}

AssetResourceBridge::~AssetResourceBridge() {}

AssetResourceBridge* AssetResourceBridge::instance = nullptr;