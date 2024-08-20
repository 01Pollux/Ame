#pragma once

#include <Graphics/EntityCompositor/EntityGpuStorage/EntityGpuStorage.hpp>

#include <EcsComponent/Lighting/BaseLight.hpp>
#include <EcsComponent/Lighting/DirectionalLight.hpp>
#include <EcsComponent/Lighting/PointLight.hpp>
#include <EcsComponent/Lighting/SpotLight.hpp>

namespace Ame::Gfx
{
    struct EntityLightInstance_EcsId
    {
        uint32_t Id = std::numeric_limits<uint32_t>::max();
    };

    struct EntityLightInstance_Data
    {
        enum class LightFlags : uint32_t
        {
            Directional = 1 << 0,
            Point       = 1 << 1,
            Spot        = 1 << 2,
            TypeMask    = Directional | Point | Spot,
        };

        Math::Vector4 Color;
        Math::Vector3 Direction{};
        float         Range = 0.f;         // 0 for directional light, Point light + spot light
        Math::Vector3 Attenuation_Angle{}; // 0 for directional light, (attenuation, 0, 0) for point light, (attenuation, angle, attenuation) for spot light
        uint32_t      Flags = 0;           // LightFlags::* flags
    };

    struct EntityGpuStorageTraits_Light
    {
        static constexpr const char* name = "LightTableTable";

        using id_container_type = EntityLightInstance_EcsId;
        using instance_type     = EntityLightInstance_Data;

        static void update(const Ecs::Entity& entity, instance_type& instance)
        {
            auto transformComponent = entity->get<Ecs::GlobalTransformComponent>();

            uint32_t typeMask = 0;
            if (auto directionalLight = entity->get<Ecs::DirectionalLightComponent>())
            {
                instance.Color = directionalLight->Color;
                typeMask       = static_cast<uint32_t>(instance_type::LightFlags::Directional);
            }
            else if (auto pointLight = entity->get<Ecs::PointLightComponent>())
            {
                instance.Color                 = pointLight->Color;
                instance.Color                 = pointLight->Color;
                instance.Range                 = pointLight->Range;
                instance.Attenuation_Angle.x() = pointLight->Attenuation;
                typeMask                       = static_cast<uint32_t>(instance_type::LightFlags::Point);
            }
            else if (auto spotLight = entity->get<Ecs::SpotLightComponent>())
            {
                instance.Color                 = spotLight->Color;
                instance.Color                 = spotLight->Color;
                instance.Range                 = spotLight->Range;
                instance.Attenuation_Angle.x() = spotLight->Attenuation;
                instance.Attenuation_Angle.y() = spotLight->Angle;
                instance.Attenuation_Angle.z() = spotLight->AngleAttenuation;
                typeMask                       = static_cast<uint32_t>(instance_type::LightFlags::Spot);
            }

            instance.Flags     = typeMask;
            instance.Direction = transformComponent->GetLookDir();
        }

        static Ecs::ObserverBuilder<> observer_create(Ecs::WorldRef world)
        {
            return world
                ->observer<>()
                .with<const Ecs::GlobalTransformComponent>()
                .and_()
                .with<const Ecs::BaseLightComponent>()
                .event(flecs::OnRemove)
                .event(flecs::OnSet);
        }
    };

    using EntityLightGpuStorage = EntityGpuStorage<EntityGpuStorageTraits_Light>;
} // namespace Ame::Gfx