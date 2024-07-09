target("SimpleLog")
    set_default(true)
    ame_utils:add_binary("Samples", "Samples/SimpleLog")
target_end()

target("SimpleWindow")
    ame_utils:add_binary("Samples", "Samples/SimpleWindow")
target_end()

--

target("Triangle")
    ame_utils:add_binary("Samples", "Samples/Triangle")
target_end()

target("RenderGraph")
    ame_utils:add_binary("Samples", "Samples/RenderGraph")
target_end()

--

target("FlappyRocket")
    ame_utils:add_binary("Samples/Games", "Samples/Games/FlappyRocket")
    ame_utils:install_assets()
target_end()
