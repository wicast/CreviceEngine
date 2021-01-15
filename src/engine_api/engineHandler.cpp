#include "engineHandler.hpp"



EngineHandler::EngineHandler(/* args */)
{
  assetManager = new AssetManager();
  rManager = new ResourceManager();

  assetResourceBridge = AssetResourceBridge::getInstance();
  assetResourceBridge->setup(rManager, assetManager);

  auto renderServer = crevice::RenderServer::getInstance();
  vkResourceBridge = crevice::VkResourceBridge::getInstance();
  vkResourceBridge->setup(rManager, renderServer->gpuRManager,
                          renderServer->vkContext);
}

EngineHandler::~EngineHandler()
{
}


void* create_engine_handler() {
    return new EngineHandler;
}