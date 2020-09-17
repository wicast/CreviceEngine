if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
end

set_languages("cxx17")

-- basic files
add_files("HelloTriangleApplication.cpp", 
     "common/*.cpp",
     "render/*.cpp", 
     "render/RenderGraph/*.cpp",
    --  "scene/*.cpp",
     "components/*.cpp")
    add_includedirs("$(projectdir)",
    "D:/SDKs/glm")

    -- 3rdParty ------------------
    -- 3rd root directory
    add_includedirs("3rd")
    -- add_files("3rd/*.cpp")

    -- volk
    add_files("3rd/volk/*.c")
    add_includedirs("3rd/volk/", "3rd/Vulkan-Headers/include")

    -- eastl
    add_files("3rd/EASTL/source/*.cpp")
    add_includedirs("3rd/EASTL/include", "3rd/EABase/include/Common")
    add_defines("EASTL_DLL=0")
    
    -- intergrate
    add_files("3rdIntergrate/*.cpp")
    add_includedirs("3rdIntergrate/")

    -- 3rdParty end ------------------

    -- CreviceStl
    -- add_files("stl/*.cpp")
    add_includedirs("stl")

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


target("MyVKLearn")
    set_kind("binary")
    add_files("main.cpp")

    
target("test")
    set_kind("binary")
    add_files("tests/testRenderGraph.cpp")