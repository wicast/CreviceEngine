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