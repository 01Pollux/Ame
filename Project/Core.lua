local public_inherit = {public = true, inherit = true}

--

ame_utils:add_library("AmeEngine", "Ame", "shared", "Source/Engine", function()
    add_defines("AME_ENGINE_EXPORT")
    
    local packages = {
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
        "ame.imgui",
    }
    for _, package in ipairs(packages) do
        add_packages(package, public_inherit)
    end
    add_packages("assimp")

    add_forceincludes(file_utils:path_from_root("Source/Engine/Public/Core/Api.hpp"), public_inherit)
end)

--

target("DotNet")
    set_kind("phony")
    set_group("Ame/Scripting")
    add_packages("dotnet", public_inherit)
target_end()

-- runtime script engine and its runtime host
ame_csharp_utils:add_library("AmeSharp.RuntimeHost", "Ame/Scripting", "shared", "Source/Scripting/AmeSharp/AmeSharp.RuntimeHost", "Plugins")
ame_utils:add_plugin("AmeSharp.Runtime", "Ame/Scripting", "Source/Scripting/AmeSharp.Runtime", "Plugins", function()
    -- add_defines("AME_CSHARP_EXPORT")
    add_defines("AME_CSHARP_STATIC_LINKING")

    add_deps("AmeEngine")
    add_deps("DotNet")
end)

-- sample for runtime script engine
ame_csharp_utils:add_library("AmeSharp.Sample", "Ame/Scripting", "shared", "Source/Scripting/AmeSharp/Sample", "Plugins")

--

ame_utils:add_library("Ame.Application", "Ame", "static", "Source/Application", function()
    add_deps("AmeEngine", public_inherit)
end)

--

ame_utils:add_plugin("AmeEditorHost", "Ame/Editor", "Source/Editor/AmeEditorHost", nil, function()
    add_defines("AME_EDITORHOST_EXPORT")
    add_defines([[AME_EDITORHOST_EXPORT_C(ReturnType, ...)=AME_EDITORHOST_API ReturnType AME_CDECL __VA_ARGS__]])

    add_deps("AmeEngine", public_inherit)

    add_forceincludes(file_utils:path_from_root("Source/Editor/AmeEditorHost/Public/EditorHost/Api.hpp"), public_inherit)
end)

-- ame_utils:add_plugin("EditorCore", "Ame/Editor", "Source/Editor/EditorCore", function()
--     add_deps("AmeEditorHost", public_inherit)
-- end)

ame_csharp_utils:add_binary("AmeEditor", "Ame/Editor", "Source/Editor/AmeEditor")
ame_csharp_utils:add_binary("AmeEditorCore", "Ame/Editor", "Source/Editor/AmeEditorCore")

-- ame_utils:add_binary("AmeEditor", "Ame/Editor", "Source/Editor/EditorApplication", function()
--     set_default(true)

--     add_deps("Ame.Application")
--     add_deps("AmeEditorHost")
--     add_deps("DotNet")

--     ame_utils:install_assets()
--     set_runargs("-p", "Shared/Assets/Projects/EmptyProject/EmptyProject.ame")
-- end)
