#pragma once

#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// Defines:
    /// LightTypeTag
    /// PointLightComponent
    /// SpotLightComponent
    /// DirectionalLightComponent
    /// </summary>
    class AME_ENGINE_API LightingEcsModule
    {
    public:
        LightingEcsModule(WorldRef world);
    };
} // namespace Ame::Ecs