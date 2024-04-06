target("Core")
    ame_header_library("Ame", "Core", "static", "Ame/Core")
    add_packages(
        "boost",
        "spdlog",
        "fmt",
        "flecs",
        "cryptopp",
        "magic_enum",
        "glm",
        "ame.concurrencpp",
        "ame.glfw",
        {public = true})
target_end()

target("Windowing")
    ame_header_library("Ame", "Windowing", "static", "Ame/Windowing")
    add_deps("Core", {public = true})
target_end()

target("Rhi")
    ame_header_library("Ame", "Rhi", "static", "Ame/Rhi")
    add_packages("nri", {public = true})
    add_deps("Windowing", {public = true})
target_end()

target("Engine")
    ame_header_library("Ame", "Engine", "static", "Ame/Engine")
    add_deps("Rhi", {public = true})
target_end()