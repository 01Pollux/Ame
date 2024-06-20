local notshared_public_inherit = {public = true, inherit = true, configs = {shared = false}}
target("Ame.Core")
    ame_utils:add_library("Ame", "static", "Ame/Core")
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
        notshared_public_inherit)
target_end()

--

target("Ame.Geometry")
    ame_utils:add_library("Ame", "static", "Ame/Geometry")
    add_deps("Ame.Core", notshared_public_inherit)
target_end()

--
target("Extensions.FreeImage")
    ame_utils:add_library("Extensions", "static", "Ame/Extensions/FreeImage")
    add_deps("Ame.Core", {public = true, inherit = true})
target_end()

--

target("Ame.Resource")
    ame_utils:add_library("Ame", "static", "Ame/Resource")
    add_deps("Ame.Geometry", notshared_public_inherit)
target_end()

target("Ame.Windowing")
    ame_utils:add_library("Ame", "static", "Ame/Windowing")
    add_deps("Ame.Core", notshared_public_inherit)
target_end()

--

target("Ame.Rhi")
    ame_utils:add_library("Ame", "static", "Ame/Rhi")
    add_packages("ame.nri", {public = true, inherit = true})
    
    add_packages("directxshadercompiler", notshared_public_inherit)
    add_packages("vulkan-memory-allocator", notshared_public_inherit)

    if is_plat("windows") then
        add_packages("d3d12-memory-allocator", notshared_public_inherit)
    end

    add_deps("Ame.Windowing", notshared_public_inherit)
target_end()

--

target("Ame.Ecs")
    ame_utils:add_library("Ame", "static", "Ame/Ecs")
    add_deps("Ame.Rhi", notshared_public_inherit)
target_end()

--

target("Ame.RenderGraph")
    ame_utils:add_library("Ame", "static", "Ame/RenderGraph")
    add_deps("Ame.Ecs", notshared_public_inherit)
target_end()

--

target("Ame.Gfx")
    ame_utils:add_library("Ame", "static", "Ame/Gfx")
    add_deps(
        "Ame.RenderGraph",
        "Ame.Ecs",
        notshared_public_inherit)
target_end()

--

target("Ame.Engine")
    ame_utils:add_library("Ame", "static", "Ame/Engine")
    add_deps("Ame.Rhi", notshared_public_inherit)
target_end()

--

ame_utils:add_tests("Rhi", "Ame/Rhi", "Ame.Rhi")
ame_utils:add_tests("Gfx", "Ame/Gfx", "Ame.Gfx")