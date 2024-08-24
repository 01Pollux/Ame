#pragma once

#include <Math/Colors.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Tag component for entities that are lights
    /// Automatically added to entities with light components
    /// </summary>
    struct LightTagComponent
    {
    };

    struct BaseLightComponent
    {
        Math::Color4 Color = Colors::c_White;
    };
} // namespace Ame::Ecs