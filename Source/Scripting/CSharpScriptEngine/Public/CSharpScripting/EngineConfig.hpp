#pragma once

#include <Core/String.hpp>

namespace Ame::Scripting
{
    struct CSScriptEngineConfig
    {
        String RuntimeConfigPath;

        [[nodiscard]] String GetRuntimeConfigPath() const
        {
            return RuntimeConfigPath + "/AmeSharp.Runtime.runtimeconfig.json";
        }

        [[nodiscard]] String GetRuntimePath() const
        {
            return RuntimeConfigPath + "/AmeSharp.Runtime.dll";
        }
    };
} // namespace Ame::Scripting