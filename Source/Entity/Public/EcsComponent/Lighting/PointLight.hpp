#pragma once

#include <EcsComponent/Lighting/BaseLight.hpp>

namespace Ame::Ecs
{
    struct PointLightComponent : public BaseLightComponent
    {
        /// <summary>
        /// Max range of light.
        /// </summary>
        float Range = 5.f;

        /// <summary>
        /// Attenuation of the light.
        /// </summary>
        float Attenuation = 1.f;
    };
} // namespace Ame::Ecs