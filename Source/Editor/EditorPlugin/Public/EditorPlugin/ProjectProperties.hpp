#pragma once

#include <Core/String.hpp>

namespace Ame::Editor
{
    namespace ProjectPropNames
    {
        inline constexpr StringView Root         = "AmeSolution";
        inline constexpr StringView Name         = "AmeSolution.Name";
        inline constexpr StringView Version      = "AmeSolution.Version";
        inline constexpr StringView StartupScene = "AmeSolution.StartupScene";
    }; // namespace ProjectPropNames
} // namespace Ame::Editor