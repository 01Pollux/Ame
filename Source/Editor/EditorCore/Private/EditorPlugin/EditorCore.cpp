#include <EditorPlugin/EditorCore.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <EditorProject/ProjectManagerImpl.hpp>
#include <EditorWindows/EditorWindowManagerImpl.hpp>

#include <Log/Logger.hpp>

namespace Ame::Editor
{
    EditorCorePlugin::EditorCorePlugin() :
        IEditorPlugin({ "EditorCore", "01Pollux", "Core plugin for the editor", __DATE__ })
    {
        s_ThisPlugin = this;
    }

    bool EditorCorePlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        if (!IEditorPlugin::OnPluginPreLoad(registry) || !registry->BindPlugin(this, "AmeSharp", true))
        {
            return false;
        }

        s_ModuleRegistry = registry;
        return true;
    }

    void EditorCorePlugin::OnPluginLoad(IModuleRegistry* registry)
    {
        IEditorPlugin::OnPluginLoad(registry);

        AME_LOG_ASSERT(registry->ExposeInterface(this, Interfaces::IID_ProjectManager, AmeCreate(ProjectManagerImpl)),
                       "Failed to expose ProjectManager");
        AME_LOG_ASSERT(
            registry->ExposeInterface(this, Interfaces::IID_EditorWindowManager, AmeCreate(EditorWindowManagerImpl)),
            "Failed to expose EditorWindowManager");
    }
} // namespace Ame::Editor

AME_PLUGIN_EXPORT(Ame::Editor::EditorCorePlugin);