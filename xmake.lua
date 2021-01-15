set_languages("cxx17")
if is_os("windows") then
    add_cxxflags("/utf-8")
end
add_rules("mode.debug")
add_rules("mode.release")
if is_mode("debug") then
    set_warnings("all")
end

includes("src")

target("vkMain")
    add_deps("cvComponents", "cvContainer", "usr")
    set_kind("binary")
    add_includedirs(".")
    add_files("main.cpp", "HelloTriangleApplication.cpp")

target("testRenderGraph")
    add_deps("flecs", "cvVkRender")
    set_kind("binary")
    add_files("tests/testRenderGraph.cpp")