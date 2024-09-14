local public_inherit = {public = true, inherit = true}

--

ame_utils:add_library("AmeEngine", "Ame", "shared", "Source/Engine", function()
    add_defines("AME_ENGINE_EXPORT")
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
    add_packages("assimp")
    add_packages("ame.imgui", public_inherit)
    add_forceincludes(file_utils:path_from_root("Source/Engine/Public/Core/Export.hpp"), public_inherit)
    add_forceincludes(file_utils:path_from_root("Source/Engine/Public/Core/Allocator.hpp"), public_inherit)
end)

--

target("DotNet")
    set_kind("phony")
    add_packages("dotnet", public_inherit)
target_end()

ame_utils:add_library("Ame.ScriptingEngine", "Ame/Scripting", "headeronly", "Source/Scripting/ScriptEngine", function()
    add_deps("AmeEngine")
end)

ame_utils:add_plugin("AmeSharp.Runtime", "Ame/Scripting", "Source/Scripting/AmeSharp.Runtime", function()
    -- add_defines("AME_CSHARP_EXPORT")
    add_defines("AME_CSHARP_STATIC_LINKING")

    add_deps("Ame.ScriptingEngine")
    add_deps("DotNet")
end)

--

ame_utils:add_library("Ame.Application", "Ame", "static", "Source/Application", function()
    add_deps("AmeEngine", public_inherit)
end)

--

ame_utils:add_library("Ame.EditorPlugin", "Ame/Editor", "static", "Source/Editor/EditorPlugin", function()
    add_deps("AmeEngine", public_inherit)
end)

ame_utils:add_plugin("EditorCore", "Ame/Editor", "Source/Editor/EditorCore", function()
    add_deps("Ame.EditorPlugin", public_inherit)
end)

ame_utils:add_binary("AmeEditor", "Ame/Editor", "Source/Editor/EditorApplication", function()
    set_default(true)

    add_deps("Ame.Application")
    add_deps("Ame.EditorPlugin")
    add_deps("DotNet")

    ame_utils:install_assets()
    set_runargs("-p", "Shared/Assets/Projects/EmptyProject/EmptyProject.ame")
end)
