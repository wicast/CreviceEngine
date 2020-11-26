/**
 * @file CVTimer.h
 * @author wicast (wicast@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-11-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#pragma once
#include <chrono>

namespace crevice {

class CVTimer {
 private:
  /* data */
  static std::chrono::high_resolution_clock::time_point startTime;
  static std::chrono::high_resolution_clock::time_point lastTime;
  static std::chrono::high_resolution_clock::time_point now;

 public:
  static void tick();
  static float delta();
  static float duration();
};

}  // namespace crevice