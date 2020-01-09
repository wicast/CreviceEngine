cmake_minimum_required(VERSION 3.15)
project(MyVKLearn)

set(CMAKE_CXX_STANDARD 17)

include_directories("D:\\SDKs\\glfw-3.3.bin.WIN64\\include" "D:\\SDKs\\glm" "D:\\VulkanSDK\\1.1.126.0\\Include" "D:\\Coding\\VK\\MyVKLearn")

add_executable(MyVKLearn
        main.cpp HelloTriangleApplication.h HelloTriangleApplication.cpp)

target_link_libraries(MyVKLearn "D:/VulkanSDK/1.1.126.0/Lib/vulkan-1.lib")

target_link_libraries(MyVKLearn "D:/SDKs/glfw-3.3.bin.WIN64/lib-mingw-w64/libglfw3.a")
