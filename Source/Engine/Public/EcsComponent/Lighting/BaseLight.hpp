#pragma once

#include <Math/Colors.hpp>

namespace Ame::Ecs
{
    struct BaseLightComponent
    {
        Math::Color4 Color = Colors::c_White;
    };

    /// <summary>
    /// Tag component for entities that are lights
    /// Automatically added to entities with light components
    /// </summary>
    struct LightTypeTag
    {
    };
} // namespace Ame::Ecs