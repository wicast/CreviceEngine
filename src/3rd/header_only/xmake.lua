target("VulkanHeaders")
    set_kind("headeronly")
    add_includedirs("Vulkan-Headers/include", {public=true})
    add_headerfiles("Vulkan-Headers/include/**.h")

target("sbt")
    set_kind("headeronly")
    add_includedirs(".", {public=true})
    add_headerfiles("stb_image.h")

target("tObjLoader")
    set_kind("headeronly")
    add_includedirs(".", {public=true})
    add_headerfiles("tiny_obj_loader.h")