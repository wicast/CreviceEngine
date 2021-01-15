#include "api.h"
#include "core/common/CVTimer.h"

extern "C" {

DLL_EXPORT void timer_tick();
DLL_EXPORT float timer_delta();
DLL_EXPORT float timer_duration();
}