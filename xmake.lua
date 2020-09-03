if is_mode("debug") then
    set_symbols("debug")
end

set_languages("cxx17")

target("MyVKLearn")
    set_kind("binary")
    add_files("*.cpp", "common/*.cpp", "render/*.cpp", "scene/*.cpp")
    add_includedirs("$(projectdir)",
    "D:/SDKs/glm")

    -- 3rdParty
    add_files("3rd/volk/*.c")
    add_includedirs("3rd/volk/", "3rd/Vulkan-Headers/include")

    -- eastl
    add_files("3rd/EASTL/source/*.cpp")
    add_includedirs("3rd/EASTL/include", "3rd/EABase/include/Common")
    add_defines("EASTL_DLL=0")

    if is_plat("mingw") then 
        add_linkdirs("D:/SDKs/glfw-3.3.2.bin.WIN64/lib-mingw-w64/")
        add_includedirs("D:/SDKs/glfw-3.3.2.bin.WIN64/include")
        add_links("glfw3")
    elseif is_plat("windows") then
        add_cxxflags("/EHsc /utf-8")
        add_linkdirs("D:/SDKs/glfw-3.3.2.bin.WIN64/lib-vc2019")
        add_includedirs("D:/SDKs/glfw-3.3.2.bin.WIN64/include")
        add_links("glfw3")
    end

    if is_plat("windows") then
        add_links( "gdi32", "user32", "shell32", "msvcrtd")
    elseif is_plat("mingw") then
        add_links("gdi32")
    end

    

 