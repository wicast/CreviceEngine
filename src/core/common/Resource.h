/**
 * @file Resource.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include <fstream>
#include <vector>
#include "eastl/shared_ptr.h"

/**
 * @brief RID
 * 
 */
typedef uint32_t RID;

//TODO use Resource class or not
/**
 * @brief 
 * 
 * @tparam T 
 */
template <class T>
struct Resource
{
  RID id;
  uint32_t type;
  eastl::shared_ptr<T> data;
};


static std::vector<char> readFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }
  size_t fileSize = (size_t)file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();

  return buffer;
}
