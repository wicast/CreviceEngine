#pragma once

#include <cstdint>
#include <string>
#include <set>
#include <unordered_set>
#include <vector>
#include "flecs.h"

struct ECSTreeItem
{
    uint64_t eid;
    std::string name;
    std::string typeString;

    std::unordered_map<uint64_t,ECSTreeItem*> children;
};

struct ECSEditorTreeItem
{
    uint64_t eid;
    std::string name;
    std::string typeString;

    std::vector<ECSEditorTreeItem*> children;
};

ECSTreeItem* dumpTree(flecs::entity curEntity) {
  auto newTree = new ECSTreeItem;
  newTree->eid = curEntity.id();
  newTree->name = curEntity.name();
  newTree->typeString = curEntity.type().str();

  for (auto children : curEntity.children()) {
    for (auto i : children) {
      // Print the child, and recursively iterate
      auto childNode = dumpTree(children.entity(i));
      newTree->children.emplace(childNode->eid,childNode);
    }
  }

  return newTree;
}

ECSEditorTreeItem* toEditorTree(ECSTreeItem* curEntity) {
  auto newTree = new ECSEditorTreeItem;
  newTree->eid = curEntity->eid;
  newTree->name = curEntity->name;
  newTree->typeString = curEntity->typeString;

  for (auto children : curEntity->children) {
      // Print the child, and recursively iterate
      auto childNode = toEditorTree(children.second);
      newTree->children.push_back(childNode);
    
  }

  return newTree;
}