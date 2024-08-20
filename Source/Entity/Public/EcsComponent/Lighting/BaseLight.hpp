#pragma once

#include <Math/Colors.hpp>

namespace Ame::Ecs
{
    struct BaseLightComponent
    {
        /// <summary>
        /// Color modulation
        /// </summary>
        Math::Color4 Color = Colors::c_White;
    };
} // namespace Ame::Ecs