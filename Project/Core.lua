local public_inherit = {public = true, inherit = true}

target("Ame.Core")
    ame_utils:add_library("Ame", "static", "Source/Core")
    add_packages(
        "boost",
        "cereal",
        "ame.diligent_core",
        "ame.kangaru",
        "spdlog",
        "fmt",
        "cryptopp",
        "magic_enum",
        "glm",
        "ame.mimalloc",
        "ame.concurrencpp",
        "ame.glfw",
        "ame.octree",
        "flecs",
        "freeimage",
        public_inherit)
target_end()

--

target("Ame.Geometry")
    ame_utils:add_library("Ame", "static", "Source/Geometry")
    add_deps("Ame.Core", public_inherit)
target_end()

target("Ame.Resource")
    ame_utils:add_library("Ame", "static", "Source/Resource")
    add_deps("Ame.Core", public_inherit)
target_end()

target("Ame.Window")
    ame_utils:add_library("Ame", "static", "Source/Window")
    add_deps("Ame.Core", public_inherit)
    add_packages("ame.imgui", public_inherit)
target_end()

--

target("Ame.Rhi")
    ame_utils:add_library("Ame", "static", "Source/Rhi")
    add_deps("Ame.Window", public_inherit)
target_end()

--

target("Ame.RenderGraph")
    ame_utils:add_library("Ame", "static", "Source/RenderGraph")
    add_deps("Ame.Rhi", public_inherit)
target_end()

target("Ame.ImGuiUtils")
    ame_utils:add_library("Ame", "static", "Source/ImGuiUtils")
    add_deps("Ame.Rhi", public_inherit)
    add_packages("ame.imgui", public_inherit)
target_end()

--

target("Ame.Ecs")
    ame_utils:add_library("Ame", "static", "Source/Entity")
    add_deps("Ame.Geometry", public_inherit)
    add_deps("Ame.RenderGraph", public_inherit)
    add_packages("assimp")
target_end()

--

target("Ame.Graphics")
    ame_utils:add_library("Ame", "static", "Source/Graphics")
    add_deps("Ame.Ecs", public_inherit)
    add_deps("Ame.ImGuiUtils", public_inherit)
target_end()

--

target("Ame.Engine")
    ame_utils:add_library("Ame", "static", "Source/Engine")
    add_deps("Ame.Graphics", public_inherit)
    add_deps("Ame.Resource", public_inherit)
target_end()

--

target("Ame.Application")
    ame_utils:add_library("Ame", "static", "Source/Application")
    add_deps("Ame.Engine")
target_end()

--

target("Ame.EditorPlugin")
    ame_utils:add_library("Ame/Editor", "static", "Source/Editor/EditorPlugin")
    add_deps("Ame.Engine", public_inherit)
    add_packages("Ame.ImGuiUtils", public_inherit)
target_end()

target("EditorCore")
    ame_utils:add_library("Ame/Editor", "shared", "Source/Editor/EditorCore")
    add_deps("Ame.EditorPlugin", public_inherit)
    ame_utils:copy_to_plugins()
target_end()

target("AmeEditor")
    set_default(true)
    ame_utils:add_library("Ame/Editor", "binary", "Source/Editor/EditorApplication")
    ame_utils:install_assets()

    add_deps("Ame.Application", public_inherit)
target_end()
