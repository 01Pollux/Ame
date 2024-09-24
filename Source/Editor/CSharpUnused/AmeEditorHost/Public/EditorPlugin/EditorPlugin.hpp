#pragma once

#include <Plugin/Plugin.hpp>

namespace Ame::Editor
{
    /// <summary>
    /// Base class for editor plugins.
    /// It enables imgui context.
    /// </summary>
    class IEditorPlugin : public IPlugin
    {
    public:
        using IPlugin::IPlugin;

        bool OnPluginPreLoad(IModuleRegistry* registry);
    };

    inline IModuleRegistry* s_ModuleRegistry = nullptr;
} // namespace Ame::Editor