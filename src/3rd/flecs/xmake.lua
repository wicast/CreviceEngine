target("flecs")
    set_kind("static")
    add_includedirs("flecs", {public=true})
    add_files("flecs/flecs.c")

-- add_flecs = function () 
--     add_deps("flecs")
--     add_defines("flecs_STATIC")
-- end