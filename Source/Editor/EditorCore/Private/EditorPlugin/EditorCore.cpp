#include <EditorPlugin/EditorCore.hpp>
#include <EditorPlugin/EditorWindowManagerImpl.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <flecs.h>

namespace Ame::Editor
{
    EditorCorePlugin::EditorCorePlugin() : IPlugin({ "EditorCore", "01Pollux", "Core plugin for the editor", __DATE__ })
    {
        ecs_set_os_api_impl();
        s_ThisPlugin = this;
    }

    bool EditorCorePlugin::OnPluginPreLoad(IModuleRegistry* registry)
    {
        if (!IPlugin::OnPluginPreLoad(registry))
        {
            return false;
        }

        s_ModuleRegistry = registry;
        return true;
    }

    void EditorCorePlugin::OnPluginLoad(IModuleRegistry* registry)
    {
        IPlugin::OnPluginLoad(registry);
        registry->ExposeInterface(
            s_ThisPlugin, Interfaces::IID_EditorWindowManager, AmeCreate(EditorWindowManagerImpl));
    }
} // namespace Ame::Editor

AME_PLUGIN_EXPORT(Ame::Editor::EditorCorePlugin);