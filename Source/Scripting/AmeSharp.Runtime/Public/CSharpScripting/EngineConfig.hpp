#pragma once

#include <Core/String.hpp>

namespace Ame::Scripting
{
    struct CSScriptEngineConfig
    {
        String RuntimeConfigPath;

        [[nodiscard]] String GetRuntimeConfigPath() const
        {
            return RuntimeConfigPath + "/AmeSharp.RuntimeHost.runtimeconfig.json";
        }

        [[nodiscard]] String GetRuntimePath() const
        {
            return RuntimeConfigPath + "/AmeSharp.RuntimeHost.dll";
        }
    };
} // namespace Ame::Scripting