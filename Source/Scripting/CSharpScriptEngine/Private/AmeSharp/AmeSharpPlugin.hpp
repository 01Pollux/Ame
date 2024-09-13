#pragma once

#include <Plugin/Plugin.hpp>

namespace Ame::Scripting
{
    class AmeSharpRuntimePlugin : public IPlugin
    {
    public:
        AmeSharpRuntimePlugin();

        bool OnPluginPreLoad(IModuleRegistry* registry);
    };
} // namespace Ame::Scripting