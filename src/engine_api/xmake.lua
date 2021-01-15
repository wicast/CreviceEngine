includes("../graphic/vk_render", "../core", "../components",
     "../3rd")

target("engineApiHeader")
    add_includedirs("..", {public=true})
    -- add_includedirs("../3rd/volk/",
    --                 "../3rd/volk/volk",
    --                 "../3rd/header_only/Vulkan-Headers/include",
    --                  {public=true})
    -- if is_plat("windows") then
    --     add_defines("VK_USE_PLATFORM_WIN32_KHR", {public=true})
    -- end
    add_deps("volk")


target("engineApi")
    add_deps("cvVkRender", "cvCore", "cvComponents", "usr")
    set_kind("shared")
    add_files("**.cpp")