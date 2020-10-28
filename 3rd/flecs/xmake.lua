target("flecs")
    set_kind("static")
    add_defines("flecs_STATIC")
    add_includedirs("flecs/include", {public=true})
    add_headerfiles("flecs/include/(flecs/*.h)","flecs/include/(flecs/*.hpp)",
        "flecs/include/(flecs/addons/*.h)", 
        "flecs/include/(flecs/modules/*.h)",
        "flecs/include/(flecs/private/*.h)"
    )
    add_headerfiles("flecs/include/*.h")
    add_files("flecs/src/**.c")

add_flecs = function () 
    add_deps("flecs")
    add_defines("flecs_STATIC")
end