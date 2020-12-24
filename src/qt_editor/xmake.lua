add_rules("mode.debug", "mode.release")
-- includes("../editor_api")
target("qtEditor")
    add_rules("qt.widgetapp")
    -- add_deps("editorApi")
    add_frameworks("QtCore")

    