#include "CVTimer.h"

namespace crevice {
std::chrono::high_resolution_clock::time_point CVTimer::startTime =
    std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point CVTimer::lastTime =
    std::chrono::high_resolution_clock::now();
std::chrono::high_resolution_clock::time_point CVTimer::now =
    std::chrono::high_resolution_clock::now();

void CVTimer::tick() {
  lastTime = now;
  now = std::chrono::high_resolution_clock::now();
}

float CVTimer::duration() {
  return std::chrono::duration<float, std::chrono::seconds::period>(now -
                                                                    startTime)
      .count();
}

float CVTimer::delta() {
  return std::chrono::duration<float, std::chrono::seconds::period>(now -
                                                                    lastTime)
      .count();
}
}  // namespace crevice
