set_languages("cxx17")
add_rules("mode.debug")
if is_mode("debug") then
-- set_warnings("all")
end

includes("3rd")

includes("containers",
        "core", 
        "graphic", 
        "components")

includes("usr")
includes("engine_api")
includes("qt_editor")
