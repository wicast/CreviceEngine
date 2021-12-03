target("VulkanHeaders")
    set_kind("headeronly")
    add_includedirs("Vulkan-Headers/include", {public=true})

target("sbt")
    set_kind("headeronly")
    add_includedirs(".", {public=true})

target("tObjLoader")
    set_kind("headeronly")
    add_includedirs(".", {public=true})