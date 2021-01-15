#include "timer.h"

extern "C" {

void timer_tick() { crevice::CVTimer::tick(); }
float timer_delta() { return crevice::CVTimer::delta(); }
float timer_duration() { return crevice::CVTimer::duration(); }
}