/**
 * @file CamControl.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#pragma once

// TODO sure this is the right way to control cam?
/**
 * @brief 
 * 
 */
struct CamControl {
  static double lastX, lastY;
  static double xoffset, yoffset;
  static bool firstMouse;

  /**
   * @brief 
   * 
   * @param xpos 
   * @param ypos 
   */
  static void updateXY(double xpos, double ypos) {
    if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }
    xoffset = xpos - lastX;
    yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
  }
};
