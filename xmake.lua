set_languages("cxx17")
includes("src")
add_rules("mode.debug")
add_rules("mode.release")
if is_mode("debug") then
    set_warnings("all")
end

target("vkMain")
    add_deps("cvComponents", "cvContainer")
    set_kind("binary")
    add_includedirs(".")
    add_files("main.cpp", "HelloTriangleApplication.cpp")

target("testRenderGraph")
    add_deps("flecs", "cvVkRender")
    set_kind("binary")
    add_files("tests/testRenderGraph.cpp")