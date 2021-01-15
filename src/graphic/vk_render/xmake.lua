includes("../../3rd")
includes("../../core")

target("cvVkRender")
    set_kind("static")
    add_deps("cvCore", "cvContainer","volk")
    add_files("**.cpp")
    add_includedirs(".")
    add_includedirs("../../",{public=true})