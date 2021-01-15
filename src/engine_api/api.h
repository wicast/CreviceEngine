#pragma once

#if defined(_WIN32)
#   define DLL_EXPORT         __declspec(dllexport)
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3))
#   define DLL_EXPORT         __attribute__((visibility("default")))
#else
#   define DLL_EXPORT
#endif

#ifdef _WIN32
#include <Windows.h>
#include <wtypes.h>
#endif

#include "containers/Container.h"


extern "C" {
/* render */
typedef void* CVVkRenderServer;

typedef CVVkRenderServer (*PFN_get_crevice_render_server_instance)(void);
typedef void (*PFN_set_render_server_container)(CVVkRenderServer, crevice::Container*);
typedef void (*PFN_render_server_init)(CVVkRenderServer);
typedef void (*PFN_render_server_wait_idle)(CVVkRenderServer server);

/* timer */
typedef void (*PFN_timer_tick)(void);
typedef float (*PFN_timer_delta)(void);
typedef float (*PFN_timer_duration)(void);


/* ecs */
typedef void* CVEcsWorld;
typedef CVEcsWorld (*PFN_cv_ecs_new_world)(void);
typedef void (*PFN_cv_ecs_setup)(CVEcsWorld);
typedef void (*PFN_cv_ecs_progress)(CVEcsWorld);
typedef void* (*PFN_cv_ecs_dump_entity_tree)(CVEcsWorld);
typedef void* (*PFN_cv_ecs_inspect_entity)(CVEcsWorld, uint32_t);

/* asset resource */
typedef void* (*PFN_cv_create_engine_handler)(void);

}
