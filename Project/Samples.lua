target("SimpleLog")
    ame_utils:add_binary("Samples/Engine", "Samples/Engine/SimpleLog")
target_end()

target("SimpleWindow")
    ame_utils:add_binary("Samples/Engine", "Samples/Engine/SimpleWindow")
target_end()

target("MultiEngine")
    ame_utils:add_binary("Samples/Engine", "Samples/Engine/MultiEngine")
target_end()

target("MultiRhi OneEngine")
    ame_utils:add_binary("Samples/Engine", "Samples/Engine/MultiRhi OneEngine")
target_end()

target("MultiRhi MultiEngine")
    ame_utils:add_binary("Samples/Engine", "Samples/Engine/MultiRhi MultiEngine")
target_end()

--

target("Triangle")
    ame_utils:add_binary("Samples/Rhi", "Samples/Rhi/Triangle")
target_end()

--

target("FlappyRocket")
    ame_utils:add_binary("Samples/Games", "Samples/Games/FlappyRocket")
    ame_utils:install_assets()
target_end()
