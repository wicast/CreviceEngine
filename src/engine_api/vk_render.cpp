#include "graphic/vk_render/RenderServer.h"
#include "vk_render.h"

extern "C" {

CVVkRenderServer get_crevice_render_server_instance() {
    return (CVVkRenderServer)crevice::RenderServer::getInstance();
}

void render_server_set_container(CVVkRenderServer server, crevice::Container* container){
    auto sr = static_cast<crevice::RenderServer*>(server);
    sr->setContainer(container);
}

void render_server_init(CVVkRenderServer server) {
    auto sr = static_cast<crevice::RenderServer*>(server);
    sr->init();
}

void render_server_wait_idle(CVVkRenderServer server) {
    auto sr = static_cast<crevice::RenderServer*>(server);
    sr->waitIdle();
}

}
