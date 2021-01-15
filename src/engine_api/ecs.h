#include "api.h"

extern "C" {
    DLL_EXPORT CVEcsWorld cv_ecs_new_world();

    DLL_EXPORT void cv_ecs_setup(CVEcsWorld world);
    DLL_EXPORT void  cv_ecs_progress(CVEcsWorld world);

    DLL_EXPORT void* cv_ecs_dump_entity_tree(CVEcsWorld world);

    DLL_EXPORT void* cv_ecs_inspect_entity(CVEcsWorld world, uint32_t entity);
}