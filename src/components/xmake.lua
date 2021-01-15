includes("../graphic/vk_render", "../3rd")

target("cvComponents")
    set_kind("static")
    add_deps("cvVkRender", "flecs")
    add_files("**.cpp")
    add_includedirs(".")
    add_includedirs("..", {public=true})
