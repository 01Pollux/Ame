target("Core")
    ame_utils:add_library("Ame", "static", "Ame/Core")

    local shared_public_inherit = {public = true, inherit = true, configs = {shared = false}}
    add_packages(
        "boost",
        "mimalloc",
        "ame.kangaru",
        "spdlog",
        "fmt",
        "freeimage",
        "flecs",
        "cryptopp",
        "magic_enum",
        "glm",
        "ame.concurrencpp",
        "ame.glfw",
        shared_public_inherit)
target_end()

--

includes("Plugins.lua")

--

target("Resource")
    ame_utils:add_library("Ame", "static", "Ame/Resource")
    add_deps("Core", {public = true, inherit = true})
target_end()

target("Windowing")
    ame_utils:add_library("Ame", "static", "Ame/Windowing")
    add_deps("Core", {public = true, inherit = true})
target_end()

--

target("Rhi")
    ame_utils:add_library("Ame", "static", "Ame/Rhi")
    add_packages(
        "ame.nri",
        "directxshadercompiler",
        {public = true, inherit = true})
    add_deps({
        "Plugins.FreeImage",
        "Windowing",
        "Resource"
    }, {public = true, inherit = true})
target_end()

--

target("Ecs")
    ame_utils:add_library("Ame", "static", "Ame/Ecs")
    add_deps("Rhi", {public = true, inherit = true})
target_end()

--

target("Gfx")
    ame_utils:add_library("Ame", "static", "Ame/Gfx")
    add_deps("Ecs", {public = true, inherit = true})
target_end()

--

target("Engine")
    ame_utils:add_library("Ame", "static", "Ame/Engine")
    add_deps({ "Gfx" }, {public = true, inherit = true})
target_end()

--

ame_utils:add_tests("Rhi", "Ame/Rhi", "Rhi")
ame_utils:add_tests("Gfx", "Ame/Gfx", "Gfx")