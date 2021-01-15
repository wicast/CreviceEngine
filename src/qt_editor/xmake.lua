add_rules("mode.debug", "mode.release")

includes("../engine_api")

target("qtEditor")
    add_rules("qt.widgetapp")
    add_frameworks("QtCore")
    add_deps("engineApiHeader", "engineApi", "flecs")
    add_files("*.cpp")
    add_files("EngineProxy.hpp", "VKContainerWidget.hpp")


target("qtEditorTest")
    add_deps("engineApiHeader")
    set_kind("binary")
    add_files("test/**.cpp")
