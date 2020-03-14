if is_mode("debug") then
    set_symbols("debug")
end

set_languages("cxx17")

target("MyVKLearn")
    set_kind("binary")
    add_files("*.cpp")
    add_includedirs("$(buildir)", "D:/SDKs/glm", "D:/VulkanSDK/1.1.126.0/Include", "D:/Coding/VK/MyVKLearn")

    add_linkdirs("D:/VulkanSDK/1.1.126.0/Lib/")
    add_links("vulkan-1")

    if is_plat("mingw") then 
        add_linkdirs("D:/SDKs/glfw-3.3.2.bin.WIN64/lib-mingw-w64/")
        add_includedirs("D:/SDKs/glfw-3.3.2.bin.WIN64/include")
        add_links("glfw3")
    elseif is_plat("windows") then
        add_cxxflags("/EHsc")
        add_linkdirs("D:/SDKs/glfw-3.3.2.bin.WIN64/lib-vc2019")
        add_includedirs("D:/SDKs/glfw-3.3.2.bin.WIN64/include")
        add_links("glfw3")
    end

    if is_plat("windows") then
        add_links( "gdi32", "user32", "shell32", "msvcrtd")
    elseif is_plat("mingw") then
        add_links("gdi32")
    end

    

 