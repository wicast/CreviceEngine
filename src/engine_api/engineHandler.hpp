#pragma once

#include "api.h"
#include "assets_manager/AssetManager.hpp"
#include "resource/ResourceManager.h"
#include "common/AssetResourceBridge.hpp"
#include "graphic/vk_render/VkResourceBridge.hpp"

class EngineHandler
{
private:
    /* data */
   
public:
    AssetManager* assetManager;
    ResourceManager* rManager;
    AssetResourceBridge* assetResourceBridge;
    crevice::VkResourceBridge* vkResourceBridge;

    EngineHandler(/* args */);
    ~EngineHandler();
};


extern "C" {
    DLL_EXPORT void* create_engine_handler();
}