#include "CoreLoader.hpp"

#include <fstream>

void CoreLoader::loadCore() {
#ifdef _WIN32

  std::ifstream f(_dllpath.c_str());
  if (!f.good()) {
    return;
  }

  LPCSTR path = _dllpath.c_str();
  hDLL = LoadLibrary(path);

  create_engine_handler = (PFN_cv_create_engine_handler)GetProcAddress(
      hDLL, "create_engine_handler");

  get_crevice_render_server_instance =
      (PFN_get_crevice_render_server_instance)GetProcAddress(
          hDLL, "get_crevice_render_server_instance");
  render_server_set_container = (PFN_set_render_server_container)GetProcAddress(
      hDLL, "render_server_set_container");
  render_server_init =
      (PFN_render_server_init)GetProcAddress(hDLL, "render_server_init");
  render_server_wait_idle = (PFN_render_server_wait_idle)GetProcAddress(
      hDLL, "render_server_wait_idle");

  timer_tick = (PFN_timer_tick)GetProcAddress(hDLL, "timer_tick");
  timer_delta = (PFN_timer_delta)GetProcAddress(hDLL, "timer_delta");
  timer_duration = (PFN_timer_duration)GetProcAddress(hDLL, "timer_duration");

  cv_ecs_new_world =
      (PFN_cv_ecs_new_world)GetProcAddress(hDLL, "cv_ecs_new_world");
  cv_ecs_setup = (PFN_cv_ecs_setup)GetProcAddress(hDLL, "cv_ecs_setup");
  cv_ecs_progress =
      (PFN_cv_ecs_progress)GetProcAddress(hDLL, "cv_ecs_progress");
  cv_ecs_dump_entity_tree = (PFN_cv_ecs_dump_entity_tree)GetProcAddress(
      hDLL, "cv_ecs_dump_entity_tree");
  cv_ecs_inspect_entity =
      (PFN_cv_ecs_inspect_entity)GetProcAddress(hDLL, "cv_ecs_inspect_entity");
#endif
}