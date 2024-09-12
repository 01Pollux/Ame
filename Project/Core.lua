local public_inherit = {public = true, inherit = true}

--

target("AmeEngine")
    add_defines("AME_ENGINE_EXPORT")
    ame_utils:add_library("Ame", "shared", "Source/Engine")
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
target_end()

--

target("Ame.ScriptingEngine")
    ame_utils:add_library("Ame/Scripting", "headeronly", "Source/Scripting/ScriptEngine")
    add_deps("AmeEngine")
target_end()

target("AmeSharp")
    -- add_defines("AME_CSHARP_EXPORT")
    add_defines("AME_CSHARP_STATIC_LINKING")
    ame_utils:add_library("Ame/Scripting", "shared", "Source/Scripting/CSharpScriptEngine")
    ame_utils:copy_to_plugins()

    add_deps("Ame.ScriptingEngine")
    add_packages("dotnet")
target_end()

--

target("Ame.Application")
    ame_utils:add_library("Ame", "static", "Source/Application")
    add_deps("AmeEngine")
target_end()

--

target("Ame.EditorPlugin")
    ame_utils:add_library("Ame/Editor", "static", "Source/Editor/EditorPlugin")
    add_deps("AmeEngine", public_inherit)
target_end()

target("EditorCore")
    ame_utils:add_library("Ame/Editor", "shared", "Source/Editor/EditorCore")
    ame_utils:copy_to_plugins()
    
    add_deps("Ame.EditorPlugin", public_inherit)
target_end()

target("AmeEditor")
    set_default(true)
    ame_utils:add_library("Ame/Editor", "binary", "Source/Editor/EditorApplication")
    ame_utils:install_assets()

    add_deps("Ame.Application")
    add_deps("Ame.EditorPlugin")

    set_runargs("-p", "Shared/Assets/Projects/EmptyProject/EmptyProject.ame")
target_end()
