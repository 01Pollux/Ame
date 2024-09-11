#include <EcsModule/LightingModule.hpp>

#include <EcsComponent/Lighting/BaseLight.hpp>
#include <EcsComponent/Lighting/DirectionalLight.hpp>
#include <EcsComponent/Lighting/PointLight.hpp>
#include <EcsComponent/Lighting/SpotLight.hpp>

namespace Ame::Ecs
{
    template<typename Ty> static void BaseLight(flecs::component<Ty> component)
    {
        component.on_set([](const Entity& entity, Ty&) { entity->add<LightTypeTag>(); });
        component.on_remove([](const Entity& entity, Ty&) { entity->remove<LightTypeTag>(); });
    }

    LightingEcsModule::LightingEcsModule(WorldRef world)
    {
        world->module<LightingEcsModule>();

        //

        world->component<LightTypeTag>();
        BaseLight(world->component<DirectionalLightComponent>());
        BaseLight(world->component<PointLightComponent>());
        BaseLight(world->component<SpotLightComponent>());
    }
} // namespace Ame::Ecs