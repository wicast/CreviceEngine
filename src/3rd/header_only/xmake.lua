target("VulkanHeaders")
    add_includedirs("Vulkan-Headers/include", {public=true})

target("sbt")
    add_includedirs(".", {public=true})

target("tObjLoader")
    add_includedirs(".", {public=true})