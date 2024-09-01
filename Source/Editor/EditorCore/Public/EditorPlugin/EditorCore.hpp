#pragma once

#include <Plugin/Plugin.hpp>

namespace Ame::Editor
{
    class EditorCorePlugin : public IPlugin
    {
    public:
        EditorCorePlugin();

        bool OnPluginPreLoad(IModuleRegistry* registry);
    };

    inline IModuleRegistry* s_ModuleRegistry = nullptr;
} // namespace Ame::Editor