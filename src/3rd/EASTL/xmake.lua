target("EABase")
    add_includedirs("EABase/include/Common", {public=true})

target("EASTL")
    add_deps("EABase")
    set_kind("static")
    add_files("EASTL/source/**.cpp")
    add_files("eastl_impl.cpp")
    add_includedirs("EASTL/include", {public=true})