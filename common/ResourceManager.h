#ifndef MYVK_RESOURCEMANAGER_H
#define MYVK_RESOURCEMANAGER_H 1

#include <map>
#include <fstream>
#include "render/Model.h"
#include "render/Texture.h"
#include "render/ShaderPack.h"

#include <random>


typedef uint32_t RID;

class ResourceManager {
    

public:
    std::map<RID, myvk::MyTexture> textures;
    std::map<RID, myvk::ShaderPack> shaders;
    std::map<RID, Mesh> meshs;

    RID addTexture(myvk::MyTexture tex) {
        RID rid = rand();
        textures.insert(std::pair<RID, myvk::MyTexture>(rid, tex));
        return rid;
    }

    RID addShaderPack(myvk::ShaderPack shader) {
        RID rid = rand();
        shaders.insert(std::pair<RID, myvk::ShaderPack>(rid, shader));
        return rid;
    }

    RID loadFromObj(std::string modelPath);

    Mesh getMesh(RID rid) {
        return meshs.find(rid)->second;
    }

    static std::vector<char> readFile(const std::string &filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
};

#endif