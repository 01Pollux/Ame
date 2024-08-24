#pragma once

#include <EcsComponent/Lighting/BaseLight.hpp>

namespace Ame::Ecs
{
    struct SpotLightComponent : BaseLightComponent
    {
        /// <summary>
        /// Max range of light.
        /// </summary>
        float Range = 5.f;

        /// <summary>
        /// Attenuation of the light.
        /// </summary>
        float Attenuation = 1.f;

        /// <summary>
        /// Angle of the cone.
        /// </summary>
        float Angle = 45.f;

        /// <summary>
        /// Attenuation of the cone.
        /// </summary>
        float AngleAttenuation = 1.f;
    };
} // namespace Ame::Ecs