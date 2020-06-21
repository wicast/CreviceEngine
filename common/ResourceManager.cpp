#include "common/ResourceManager.h"

// #define TINYOBJLOADER_IMPLEMENTATION

// #include <tiny_obj_loader.h>

// RID ResourceManager::loadFromObj(std::string modelPath) {
//   tinyobj::attrib_t attrib;
//   std::vector<tinyobj::shape_t> shapes;
//   std::vector<tinyobj::material_t> materials;
//   std::string warn, err;

//   Mesh mesh;

//   if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
//                         modelPath.c_str())) {
//     throw std::runtime_error(warn + err);
//   }

//   std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

//   for (const auto &shape : shapes) {
//     for (const auto &index : shape.mesh.indices) {
//       Vertex vertex = {};
//       vertex.pos = {attrib.vertices[3 * index.vertex_index + 0],
//                     attrib.vertices[3 * index.vertex_index + 1],
//                     attrib.vertices[3 * index.vertex_index + 2]};

//       vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
//                          1.0f - attrib.texcoords[2 * index.texcoord_index +
//                          1]};

//       vertex.normal = {attrib.normals[3 * index.normal_index + 0],
//                        attrib.normals[3 * index.normal_index + 1],
//                        attrib.normals[3 * index.normal_index + 2]};

//       vertex.color = {1.0f, 1.0f, 1.0f};
//       if (uniqueVertices.count(vertex) == 0) {
//         uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
//         mesh.vertices.push_back(vertex);
//       }

//       mesh.indices.push_back(uniqueVertices[vertex]);
//     }
//   }

//   RID rid = rand();
//   this->meshs.insert(std::pair<RID, Mesh>(rid, mesh));

//   return rid;
// }