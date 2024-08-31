#include <EcsModule/LightingModule.hpp>

#include <EcsComponent/Lighting/BaseLight.hpp>
#include <EcsComponent/Lighting/DirectionalLight.hpp>
#include <EcsComponent/Lighting/PointLight.hpp>
#include <EcsComponent/Lighting/SpotLight.hpp>

namespace Ame::Ecs
{
    template<typename Ty> static void BaseLight(flecs::component<Ty> component)
    {
        component.on_set([](const Entity& entity, Ty&) { entity->add<LightTagComponent>(); });
        component.on_remove([](const Entity& entity, Ty&) { entity->remove<LightTagComponent>(); });
    }

    LightingEcsModule::LightingEcsModule(WorldRef world)
    {
        world->module<LightingEcsModule>();

        //

        world->component<LightTagComponent>("Ame.LightTagComponent");
        BaseLight(world->component<DirectionalLightComponent>("Ame.DirectionalLightComponent"));
        BaseLight(world->component<PointLightComponent>("Ame.PointLightComponent"));
        BaseLight(world->component<SpotLightComponent>("Ame.SpotLightComponent"));
    }
} // namespace Ame::Ecs