#include <EditorPlugin/EditorCore.hpp>

namespace Ame::Editor
{
    EditorCorePlugin::EditorCorePlugin() : IPlugin({ "EditorCore", "01Pollux", "Core plugin for the editor", __DATE__ })
    {
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
} // namespace Ame::Editor