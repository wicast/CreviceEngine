target("cvContainer")
    add_deps("glfw3", "volk")
    set_kind("static")
    add_files("**.cpp")
    add_includedirs(".")
    add_includedirs("..",{public=true})