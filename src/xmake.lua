set_languages("cxx17")
add_rules("mode.debug")
if is_mode("debug") then
    -- set_warnings("all")
end

includes("3rd")

-- includes("qt_editor")
-- includes("editor_api")

target("cvContainer")
    add_deps("glfw3", "volk")
    set_kind("static")
    add_files("containers/**.cpp")
    add_includedirs("containers/",{public=true})

target("cvcore")
    set_kind("static")
    add_deps("EASTL", "glm", "tObjLoader")
    add_files("core/**.cpp")
    add_includedirs("core/",{public=true})


target("cvVkRender")
    set_kind("static")
    add_deps("cvcore", "cvContainer","volk")
    add_files("graphic/vk_render/**.cpp")
    add_includedirs("graphic/vk_render/", ".",{public=true})

target("cvComponents")
    set_kind("static")
    add_deps("cvVkRender", "flecs")
    add_files("components/**.cpp")
    add_includedirs(".", {public=true})


target("editorApi")
    add_deps("cvVkRender", "cvcore")
    set_kind("shared")