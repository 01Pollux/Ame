ame_utils:add_binary("HelloWorld", "Examples", "Source/Examples/HelloWorld")

ame_utils:add_binary("SimpleWindow", "Examples", "Source/Examples/SimpleWindow")

ame_utils:add_binary("SimpleRhi", "Examples", "Source/Examples/SimpleRhi")

ame_utils:add_binary("PluginExample", "Examples/Plugin", "Source/Examples/PluginExample", function()
    add_deps("Ame.Application")
end)

ame_utils:add_plugin("PluginHost", "Examples/Plugin", "Source/Examples/PluginHost", "Plugins")
