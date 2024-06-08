target("Plugins.FreeImage")
    ame_utils:add_library("Plugins", "static", "Ame/Plugins/FreeImage")
    add_deps("Ame.Core", {public = true, inherit = true})
target_end()