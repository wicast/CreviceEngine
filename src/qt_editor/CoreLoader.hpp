#pragma once

#include <string>
#ifdef _WIN32
#include <Windows.h>
#include <wtypes.h>
#endif

#include "engine_api/api.h"

class CoreLoader {
 private:
  // TODO get coreDll from path system
  std::string _dllpath =
      "engineApi.dll";
#ifdef _WIN32
  HINSTANCE hDLL;
#endif

 public:
  PFN_cv_create_engine_handler create_engine_handler;

  //TODO there will multiple renderserver
  PFN_get_crevice_render_server_instance get_crevice_render_server_instance;
  PFN_set_render_server_container render_server_set_container;
  PFN_render_server_init render_server_init;
  PFN_render_server_wait_idle render_server_wait_idle;


  PFN_timer_tick timer_tick;
  PFN_timer_delta timer_delta;
  PFN_timer_duration timer_duration;

  PFN_cv_ecs_new_world cv_ecs_new_world;
  PFN_cv_ecs_setup cv_ecs_setup;
  PFN_cv_ecs_progress cv_ecs_progress;
  PFN_cv_ecs_dump_entity_tree cv_ecs_dump_entity_tree;
  PFN_cv_ecs_inspect_entity cv_ecs_inspect_entity;


  void loadCore();

  void unloadCore();

  void reloadCore();
};
