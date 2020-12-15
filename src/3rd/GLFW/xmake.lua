if is_plat("windows") then
    add_links( "gdi32", "user32", "shell32", "msvcrtd")
elseif is_plat("mingw") then
    add_links("gdi32")
end

target("glfw3")
    add_includedirs(".", {public=true})
    add_includedirs("glfw/include", {public=true})

    set_kind("static")
    add_files("glfw/src/context.c", 
        "glfw/src/init.c", 
        "glfw/src/input.c", 
        "glfw/src/monitor.c", 
        "glfw/src/vulkan.c", 
        "glfw/src/window.c")

    if is_plat("windows") then 
        add_defines("_GLFW_WIN32=1")
        add_files("glfw/src/win32_*.c",
            "glfw/src/wgl_context.c",
            "glfw/src/egl_context.c",
            "glfw/src/osmesa_context.c")
    end