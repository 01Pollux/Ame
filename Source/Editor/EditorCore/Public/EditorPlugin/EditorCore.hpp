#pragma once

#include <Plugin/EditorPlugin.hpp>

namespace Ame::Editor
{
    class EditorCorePlugin : public IEditorPlugin
    {
    public:
        EditorCorePlugin();

        bool OnPluginPreLoad(IModuleRegistry* registry);
        void OnPluginLoad(IModuleRegistry* registry);
    };

    inline EditorCorePlugin* s_ThisPlugin = nullptr;
} // namespace Ame::Editor