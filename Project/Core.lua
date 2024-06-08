target("Ame.Core")
    ame_utils:add_library("Ame", "static", "Ame/Core")

    local shared_public_inherit = {public = true, inherit = true, configs = {shared = false}}
    add_packages(
        "boost",
        "ame.mimalloc",
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

target("Ame.Geometry")
    ame_utils:add_library("Ame", "static", "Ame/Geometry")
    add_deps("Ame.Core", {public = true, inherit = true})
target_end()

--

includes("Plugins.lua")

--

target("Ame.Resource")
    ame_utils:add_library("Ame", "static", "Ame/Resource")
    add_deps("Ame.Core", {public = true, inherit = true})
target_end()

target("Ame.Windowing")
    ame_utils:add_library("Ame", "static", "Ame/Windowing")
    add_deps("Ame.Core", {public = true, inherit = true})
target_end()

--

target("Ame.Rhi")
    ame_utils:add_library("Ame", "static", "Ame/Rhi")
    add_packages(
        "ame.nri",
        "directxshadercompiler",
        {public = true, inherit = true})
    add_deps({
        "Plugins.FreeImage",
        "Ame.Windowing",
        "Ame.Resource"
    }, {public = true, inherit = true})
target_end()

--

target("Ame.Ecs")
    ame_utils:add_library("Ame", "static", "Ame/Ecs")
    add_deps("Ame.Rhi", {public = true, inherit = true})
target_end()

--

target("Ame.Gfx")
    ame_utils:add_library("Ame", "static", "Ame/Gfx")
    add_deps("Ame.Ecs", {public = true, inherit = true})
target_end()

--

target("Ame.Engine")
    ame_utils:add_library("Ame", "static", "Ame/Engine")
    add_deps({ "Ame.Gfx" }, {public = true, inherit = true})
target_end()

--

ame_utils:add_tests("Rhi", "Ame/Rhi", "Ame.Rhi")
ame_utils:add_tests("Gfx", "Ame/Gfx", "Ame.Gfx")