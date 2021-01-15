#include "ecs.h"
#include "3rd/flecs/flecs/flecs.h"
#include "components/ecs_register_comp.h"
#include "core/systems/default_render_sys.h"
#include "core/systems/sys_asset_loader.hpp"
#include "usr/adhoc_codegen/cust_system.h"
#include "usr/user_data_setup.hpp"

CVEcsWorld cv_ecs_new_world() { return static_cast<void*>(new flecs::world()); }

void cv_ecs_setup(CVEcsWorld world) {
    auto ecs = static_cast<flecs::world*>(world);
    ecs_reg_comp(*ecs);
    SetRenderHandler(*ecs);
    setupDefaultSys(*ecs);
    setAssetResourceSystems(*ecs);

    usr_ecs_scene_setup(*ecs);
    setCustSys(*ecs);

}

void cv_ecs_progress(CVEcsWorld world) {
    auto ecs = static_cast<flecs::world*>(world);
    ecs->progress();
}

void* cv_ecs_dump_entity_tree(CVEcsWorld world) {
    return nullptr;
}