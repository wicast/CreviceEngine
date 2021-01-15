#include "engine_api/api.h"

int main(int argc, char const *argv[]) {
  HINSTANCE hDLL;
  hDLL = LoadLibrary(
      TEXT("D:/Coding/VK/MyVKLearn/build/windows/x64/debug/editorApi.dll"));

  PFN_get_crevice_render_server_instance get_crevice_render_server_instance =
      (PFN_get_crevice_render_server_instance)GetProcAddress(hDLL, "get_crevice_render_server_instance");

  PFN_set_render_server_container render_server_set_container =  (PFN_set_render_server_container)GetProcAddress(hDLL, "render_server_set_container");

  CVVkRenderServer a = get_crevice_render_server_instance();

  // crevice::Container container;

  // render_server_set_container(a, &container);
  return 0;
}
