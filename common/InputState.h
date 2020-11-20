#pragma once

#include "stl/CreviceHashSet.h"

#define CV_KEY_W 87
#define CV_KEY_S 83
#define CV_KEY_A 65
#define CV_KEY_D 68
#define CV_KEY_E 69
#define CV_KEY_Q 81

namespace crevice {
struct InputState {
  // keyboard
  static crevice::HashSet<uint32_t> states;

  static bool getKey(uint32_t key) { return states.find(key) != states.end(); }

  // mouse
  static double xpos, ypos;

  static void mouse_callback(double x, double y) {
    xpos = x;
    ypos = y;
  }
};
}  // namespace crevice
