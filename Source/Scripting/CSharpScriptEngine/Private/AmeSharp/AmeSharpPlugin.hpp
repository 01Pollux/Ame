#pragma once

#include <Plugin/Plugin.hpp>

namespace Ame::Scripting
{
    class AmeSharpPlugin : public IPlugin
    {
    public:
        AmeSharpPlugin();

        bool OnPluginPreLoad(IModuleRegistry* registry);
    };
} // namespace Ame::Scripting