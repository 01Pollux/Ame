#include <EditorPlugin/EditorCore.hpp>
#include <EditorPlugin/EditorWindowManagerImpl.hpp>
#include <Plugin/ModuleRegistry.hpp>

namespace Ame::Editor
{
    EditorCorePlugin::EditorCorePlugin() :
        IEditorPlugin({ "EditorCore", "01Pollux", "Core plugin for the editor", __DATE__ })
    {
        s_ThisPlugin = this;
    }

    bool EditorCorePlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        if (!IEditorPlugin::OnPluginPreLoad(registry))
        {
            return false;
        }

        s_ModuleRegistry = registry;
        return true;
    }

    void EditorCorePlugin::OnPluginLoad(IModuleRegistry* registry)
    {
        IEditorPlugin::OnPluginLoad(registry);
        registry->ExposeInterface(
            s_ThisPlugin, Interfaces::IID_EditorWindowManager, AmeCreate(EditorWindowManagerImpl));
    }
} // namespace Ame::Editor

AME_PLUGIN_EXPORT(Ame::Editor::EditorCorePlugin);