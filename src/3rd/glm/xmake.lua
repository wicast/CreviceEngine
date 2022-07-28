target("glm")
    set_kind("headeronly")
    add_includedirs("glm",{public=true})
    add_headerfiles("glm/**.hpp")