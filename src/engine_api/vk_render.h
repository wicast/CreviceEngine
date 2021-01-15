#pragma once

#include "api.h"
#include "containers/Container.h"
#include "volk_imp.h"

extern "C" {
DLL_EXPORT CVVkRenderServer get_crevice_render_server_instance();

DLL_EXPORT void render_server_set_container(CVVkRenderServer server, crevice::Container* container);

DLL_EXPORT void render_server_init(CVVkRenderServer server);

DLL_EXPORT void render_server_wait_idle(CVVkRenderServer server);

}
