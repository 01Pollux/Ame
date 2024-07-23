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
    if not is_mode("release") then
        add_packages("ame.imgui", notshared_public_inherit)
    end
target_end()

--

target("Ame.Rhi")
    ame_utils:add_library("Ame", "static", "Source/Rhi")
    add_deps("Ame.Window", notshared_public_inherit)
target_end()

--

target("Ame.ImGuiUtils")
    ame_utils:add_library("Ame", "static", "Source/ImGuiUtils")
    add_deps("Ame.Rhi", notshared_public_inherit)
    add_packages("ame.imgui", notshared_public_inherit)
target_end()

--

target("Ame.Graphics")
    ame_utils:add_library("Ame", "static", "Source/Graphics")
    add_deps("Ame.Rhi", notshared_public_inherit)
    if not is_mode("release") then
        add_deps("Ame.ImGuiUtils", notshared_public_inherit)
    end
target_end()

--

target("Ame.Engine")
    ame_utils:add_library("Ame", "static", "Source/Engine")
    add_deps("Ame.Graphics", notshared_public_inherit)
target_end()

--

target("Ame.Application")
    ame_utils:add_library("Ame", "static", "Source/Application")
    add_deps("Ame.Engine", notshared_public_inherit)
target_end()

--

target("Ame.EditorPlugin")
    add_defines("AME_DYNAMIC_LINKING", {public = true, inherit = true})
    add_defines("AME_EXPORT_API", {public = false, inherit = false})
    ame_utils:add_library("Ame", "shared", "Source/EditorPlugin")
    add_deps("Ame.Engine", notshared_public_inherit)
    add_packages("Ame.ImGuiUtils", notshared_public_inherit)
target_end()

target("AmeEditor")
    ame_utils:add_library("Ame", "binary", "Source/EditorApplication")
    add_deps("Ame.EditorPlugin", notshared_public_inherit)
    add_deps("Ame.Application", notshared_public_inherit)
target_end()
