local notshared_public_inherit = {public = true, inherit = true, configs = {shared = false}}
target("Ame.Core")
    ame_utils:add_library("Ame", "static", "Source/Core")
    add_packages(
        "boost",
        "ame.mimalloc",
        "ame.diligent_core",
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
        notshared_public_inherit)
target_end()

--

target("Ame.Window")
    ame_utils:add_library("Ame", "static", "Source/Window")
    add_deps("Ame.Core", notshared_public_inherit)
target_end()

--

target("Ame.Rhi")
    ame_utils:add_library("Ame", "static", "Source/Rhi")
    add_deps("Ame.Window", notshared_public_inherit)
target_end()

--

target("Ame.Engine")
    ame_utils:add_library("Ame", "static", "Source/Engine")
    add_deps("Ame.Rhi", notshared_public_inherit)
target_end()
