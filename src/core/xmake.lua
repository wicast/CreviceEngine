includes("../3rd")

target("cvCore")
    set_kind("static")
    add_deps("EASTL", "glm", "tObjLoader")
    add_files("**.cpp")
    add_includedirs(".",{public=true})