-- include vulkan header
includes("../header_only")

target("volk")
    add_deps("VulkanHeaders")
    add_files("volk/volk.c")
    set_kind("static")
    add_defines("VK_NO_PROTOTYPES",{public=true})
    add_defines("VK_USE_PLATFORM_WIN32_KHR",{public=true})

    -- if is_plat("windows") then
    -- end

    add_includedirs("volk", ".",{public=true})