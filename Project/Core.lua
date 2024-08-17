local nonshared_public_inherit = {public = true, inherit = true, configs = {shared = false}}
local shared_public_inherit = {public = true, inherit = true, configs = {shared = true}}

target("Ame.Core")
    ame_utils:add_library("Ame", "static", "Source/Core")

    add_packages(
        "boost",
        "cereal",
        "ame.mimalloc",
        "ame.diligent_core",
        "ame.kangaru",
        "spdlog",
        "fmt",
        "flecs",
        "cryptopp",
        "magic_enum",
        "glm",
        "ame.concurrencpp",
        "ame.glfw",
        nonshared_public_inherit)
    add_packages(
        "freeimage",
        shared_public_inherit)
target_end()

--

target("Ame.Geometry")
    ame_utils:add_library("Ame", "static", "Source/Geometry")
    add_deps("Ame.Core", nonshared_public_inherit)
target_end()

target("Ame.Resource")
    ame_utils:add_library("Ame", "static", "Source/Resource")
    add_deps("Ame.Core", nonshared_public_inherit)
target_end()

target("Ame.Window")
    ame_utils:add_library("Ame", "static", "Source/Window")
    add_deps("Ame.Core", nonshared_public_inherit)
    if not is_mode("release") then
        add_packages("ame.imgui", nonshared_public_inherit)
    end
target_end()

--

target("Ame.Rhi")
    ame_utils:add_library("Ame", "static", "Source/Rhi")
    add_deps("Ame.Window", nonshared_public_inherit)
target_end()

--

target("Ame.RenderGraph")
    ame_utils:add_library("Ame", "static", "Source/RenderGraph")
    add_deps("Ame.Rhi", nonshared_public_inherit)
target_end()

target("Ame.ImGuiUtils")
    ame_utils:add_library("Ame", "static", "Source/ImGuiUtils")
    add_deps("Ame.Rhi", nonshared_public_inherit)
    add_packages("ame.imgui", nonshared_public_inherit)
target_end()

--

target("Ame.Ecs")
    ame_utils:add_library("Ame", "static", "Source/Entity")
    add_deps("Ame.Geometry", nonshared_public_inherit)
    add_deps("Ame.RenderGraph", nonshared_public_inherit)
    add_packages("assimp")
target_end()

--

target("Ame.Graphics")
    ame_utils:add_library("Ame", "static", "Source/Graphics")
    add_deps("Ame.Ecs", nonshared_public_inherit)
    if not is_mode("release") then
        add_deps("Ame.ImGuiUtils", nonshared_public_inherit)
    end
target_end()

--

target("Ame.Engine")
    ame_utils:add_library("Ame", "static", "Source/Engine")
    add_deps("Ame.Graphics", nonshared_public_inherit)
    add_deps("Ame.Resource", nonshared_public_inherit)
target_end()

--

target("Ame.Application")
    ame_utils:add_library("Ame", "static", "Source/Application")
    add_deps("Ame.Engine")
target_end()

--

target("Ame.EditorPlugin")
    ame_utils:add_library("Ame", "static", "Source/EditorPlugin")
    add_deps("Ame.Engine", nonshared_public_inherit)
    add_packages("Ame.ImGuiUtils", nonshared_public_inherit)
target_end()

target("AmeEditor")
    set_default(true)
    ame_utils:add_library("Ame", "binary", "Source/EditorApplication")
    ame_utils:install_assets()

    add_deps("Ame.Application", nonshared_public_inherit)
    add_deps("Ame.EditorPlugin", nonshared_public_inherit)
target_end()
