/**
 * @file InputState.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

#include "stl/CreviceHashSet.h"

#define CV_KEY_W 87
#define CV_KEY_S 83
#define CV_KEY_A 65
#define CV_KEY_D 68
#define CV_KEY_E 69
#define CV_KEY_Q 81

namespace crevice {
/**
 * @brief Input Collector
 * 
 */
struct InputState {
  /**
   * @brief keyboard
   * 
   */
  static crevice::HashSet<uint32_t> states;

  /**
   * @brief Get the Key state
   * 
   * @param key 
   * @return true 
   * @return false 
   */
  static bool getKey(uint32_t key) { return states.find(key) != states.end(); }

  /**
   * @brief mouse position
   * 
   */
  static double xpos, ypos;

  /**
   * @brief mouse callback
   * 
   * @param x 
   * @param y 
   */
  static void mouse_callback(double x, double y) {
    xpos = x;
    ypos = y;
  }
};
}  // namespace crevice
