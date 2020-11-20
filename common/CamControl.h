#pragma once

// TODO sure this is the right way to control cam?
struct CamControl {
  static double lastX, lastY;
  static double xoffset, yoffset;
  static bool firstMouse;

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
