includes("../header_only")

target("volk")
    add_deps("VulkanHeaders")
    add_files("volk/volk.c")
    set_kind("static")
    add_defines("VK_NO_PROTOTYPES",{public=true})
    add_includedirs("volk", ".",{public=true})