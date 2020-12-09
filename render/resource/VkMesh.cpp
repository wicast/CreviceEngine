#include "render/resource/VkMesh.hpp"
#include "render/RenderServer.h"


VkMesh::~VkMesh() {
    auto server = crevice::RenderServer::getInstance();
    auto vkContext = server->vkContext;

    vkDestroyBuffer(vkContext->device, indexBuffer, nullptr);
    vkFreeMemory(vkContext->device, indexBufferMemory, nullptr);

    vkDestroyBuffer(vkContext->device, vertexBuffer, nullptr);
    vkFreeMemory(vkContext->device, vertexBufferMemory, nullptr);
}