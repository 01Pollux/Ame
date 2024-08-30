target("HelloWorld")
    ame_utils:add_binary("Examples", "Source/Examples/HelloWorld")
target_end()

target("SimpleWindow")
    ame_utils:add_binary("Examples", "Source/Examples/SimpleWindow")
target_end()

target("SimpleRhi")
    ame_utils:add_binary("Examples", "Source/Examples/SimpleRhi")
target_end()

target("PluginExample")
    ame_utils:add_binary("Examples/Plugin", "Source/Examples/PluginExample")
    add_deps("Ame.Application")
target_end()

target("PluginHost")
    ame_utils:add_library("Examples/Plugin", "shared", "Source/Examples/PluginHost")
    add_deps("Ame.Engine")
target_end()
