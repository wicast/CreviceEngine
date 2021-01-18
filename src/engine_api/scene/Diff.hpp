#pragma once

#include <algorithm>

#include "ECSTreeItem.hpp"

enum DiffType { Add, Remove, Update };

struct ECSDiffItem {
  uint64_t eid;
  std::string name;
  std::string typeString;
};

struct DiffNode {
  DiffType type;
  ECSDiffItem data;
};

using DiffSet = std::unordered_map<uint64_t, DiffNode>;

// key is parent of change
using DiffSets = std::unordered_map<uint64_t, DiffSet>;

template <class T>
void findNotInSet(T& mainSet, T& compSet, T& resSet) {
  resSet = compSet;
  for (auto i : mainSet) {
    resSet.erase(i);
  }
}

template <class K, class V>
void childrenMapToSet(std::unordered_map<K, V>& m, std::set<K>& s) {
  for (auto i : m) {
    s.emplace(i.second->eid);
  }
}

void collectNewNode(ECSTreeItem* node, DiffSets* diffSets) {
  for (auto c : node->children) {
    auto ndiff = DiffNode{};
    ndiff.type = DiffType::Add;
    ndiff.data.eid = c.first;
    ndiff.data.name = c.second->name;
    ndiff.data.typeString = c.second->typeString;

    DiffSet s;
    if (diffSets->count(node->eid) != 0) {
      s = diffSets->at(node->eid);
    }
    s[c.first] = ndiff;
    (*diffSets)[node->eid] = s;

    collectNewNode(c.second, diffSets);
  }
}

void diffTree(ECSTreeItem* oldRoot, ECSTreeItem* newRoot, DiffSets* diffSets) {
  if (oldRoot == nullptr) {
    // error
  }
  if (newRoot == nullptr) {
    // error
  }

  std::set<uint64_t> resSet;
  std::set<uint64_t> nChild;
  std::set<uint64_t> oChild;

  childrenMapToSet(newRoot->children, nChild);
  childrenMapToSet(oldRoot->children, oChild);

  // Added
  findNotInSet(oChild, nChild, resSet);
  for (auto i : resSet) {
    auto ndiff = DiffNode{};
    ndiff.type = DiffType::Add;
    ndiff.data.eid = i;
    ndiff.data.name = newRoot->children.at(i)->name;
    ndiff.data.typeString = newRoot->children.at(i)->typeString;

    DiffSet s;
    if (diffSets->count(newRoot->eid) != 0) {
      s = diffSets->at(newRoot->eid);
    }
    s[i] = ndiff;
    (*diffSets)[newRoot->eid] = s;
  }

  for (auto i : resSet) {
    /* code */
    collectNewNode(newRoot->children.at(i), diffSets);
  }

  // Update
  std::set<uint64_t> interset;

  findNotInSet(resSet, nChild, interset);
  for (auto i : interset) {
    if (newRoot->children.at(i)->name != oldRoot->children.at(i)->name ||
        newRoot->children.at(i)->typeString !=
            oldRoot->children.at(i)->typeString) {
      auto ndiff = DiffNode{};
      ndiff.type = DiffType::Update;
      ndiff.data.eid = i;
      ndiff.data.name = newRoot->children.at(i)->name;
      ndiff.data.typeString = newRoot->children.at(i)->typeString;

      DiffSet s;
      if (diffSets->count(newRoot->eid) != 0) {
        s = diffSets->at(newRoot->eid);
      }
      s[i] = ndiff;
      (*diffSets)[newRoot->eid] = s;
    }
    diffTree(oldRoot->children[i], newRoot->children[i], diffSets);
  }

  // Remove
  resSet.clear();
  findNotInSet(nChild, oChild, resSet);
  for (auto i : resSet) {
    auto ndiff = DiffNode{};
    ndiff.type = DiffType::Remove;
    ndiff.data.eid = i;

    DiffSet s;
    if (diffSets->count(newRoot->eid) != 0) {
      s = diffSets->at(newRoot->eid);
    }
    s[i] = ndiff;
    (*diffSets)[newRoot->eid] = s;
  }
}

void patchRemove(ECSEditorTreeItem* parent, ECSEditorTreeItem* nodeBeRemove) {
  for (auto c : nodeBeRemove->children) {
    patchRemove(nodeBeRemove, c);
  }

  // TODO faster remove
  parent->children.erase(std::remove(parent->children.begin(),
                                     parent->children.end(), nodeBeRemove),
                         parent->children.end());
  delete nodeBeRemove;
}

void patchAdd(ECSEditorTreeItem* root, uint64_t newEid, DiffSets* diffSets) {
  auto diff = diffSets->at(root->eid).at(newEid);
  auto newTreeNode = new ECSEditorTreeItem;
  newTreeNode->eid = newEid;
  newTreeNode->name = diff.data.name;
  newTreeNode->typeString = diff.data.typeString;
  root->children.push_back(newTreeNode);

  if (diffSets->count(newEid) != 0) {
    for (auto i : (*diffSets)[newEid]) {
      patchAdd(newTreeNode, i.first, diffSets);
    }
  }
}

void patchTree(ECSEditorTreeItem* oldRoot, DiffSets* diffSets) {
  if (diffSets->count(oldRoot->eid) != 0) {
    auto nodeDiffs = (*diffSets).at(oldRoot->eid);
    for (auto c : oldRoot->children) {
      // Update and remove
      if (nodeDiffs.count(c->eid) != 0) {
        auto diff = nodeDiffs[c->eid];
        if (diff.type == DiffType::Remove) {
          patchRemove(oldRoot, c);
          nodeDiffs.erase(c->eid);
        } else if (diff.type == DiffType::Update) {
          c->name = diff.data.name;
          c->typeString = diff.data.typeString;
          // recursive
          nodeDiffs.erase(c->eid);
          for (auto cc : c->children) {
            patchTree(cc, diffSets);
          }
        }
      }
    }

    // Add
    for (auto diff : nodeDiffs) {
      patchAdd(oldRoot, diff.first, diffSets);
      nodeDiffs.erase(diff.first);
    }
  }
}