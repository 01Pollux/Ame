target("Core")
    ame_header_library("Ame", "Core", "static", "Ame/Core")
    add_packages(
        "boost",
        "mimalloc",
        "ame.kangaru",
        "spdlog",
        "fmt",
        "flecs",
        "cryptopp",
        "magic_enum",
        "glm",
        "ame.concurrencpp",
        "ame.glfw",
        {public = true, inherit = true, configs = {shared = false}})
target_end()

--

target("Resource")
    ame_header_library("Ame", "Resource", "static", "Ame/Resource")
    add_deps("Core", {public = true, inherit = true})
target_end()

target("Windowing")
    ame_header_library("Ame", "Windowing", "static", "Ame/Windowing")
    add_deps("Core", {public = true, inherit = true})
target_end()

--

target("Rhi")
    ame_header_library("Ame", "Rhi", "static", "Ame/Rhi")
    add_packages(
        "ame.nri",
        "directxshadercompiler",
        {public = true, inherit = true})
    add_deps("Windowing", {public = true, inherit = true})
target_end()

--

target("Ecs")
    ame_header_library("Ame", "Ecs", "static", "Ame/Ecs")
    add_deps("Rhi", {public = true, inherit = true})
target_end()

--

target("Gfx")
    ame_header_library("Ame", "Gfx", "static", "Ame/Gfx")
    add_deps({ "Ecs", "Resource" }, {public = true, inherit = true})
target_end()

--

target("Engine")
    ame_header_library("Ame", "Engine", "static", "Ame/Engine")
    add_deps({ "Gfx" }, {public = true, inherit = true})
target_end()