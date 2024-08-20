#include <EcsModule/LightingModule.hpp>

#include <EcsComponent/Lighting/BaseLight.hpp>
#include <EcsComponent/Lighting/DirectionalLight.hpp>
#include <EcsComponent/Lighting/PointLight.hpp>
#include <EcsComponent/Lighting/SpotLight.hpp>

namespace Ame::Ecs
{
    LightingEcsModule::LightingEcsModule(
        WorldRef world)
    {
        world->module<LightingEcsModule>();

        //

        world->component<BaseLightComponent>("Ame.BaseLightComponent");
        world->component<DirectionalLightComponent>("Ame.DirectionalLightComponent")
            .is_a<BaseLightComponent>();
        world->component<PointLightComponent>("Ame.PointLightComponent")
            .is_a<BaseLightComponent>();
        world->component<SpotLightComponent>("Ame.SpotLightComponent")
            .is_a<BaseLightComponent>();
    }
} // namespace Ame::Ecs