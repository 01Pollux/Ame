#pragma once

#include <Graphics/EntityCompositor/EntityGpuStorage/EntityGpuStorage.hpp>

#include <EcsComponent/Math/Transform.hpp>
#include <EcsComponent/Renderables/BaseRenderable.hpp>
#include <EcsComponent/Lighting/BaseLight.hpp>

namespace Ame::Gfx
{
    struct EntityTransform_EcsId
    {
        uint32_t Id = std::numeric_limits<uint32_t>::max();
    };

    struct EntityGpuStorageTraits_Transform
    {
        static constexpr const char* name = "TransformTable";

        using id_container_type = EntityTransform_EcsId;
        using instance_type     = Math::Matrix4x4;

        static void update(const Ecs::Entity& entity, instance_type& instance)
        {
            auto transform = entity->get<Ecs::GlobalTransformComponent>();
            instance       = transform->ToMat4x4();
        }

        static auto observer_create(Ecs::WorldRef world)
        {
            return world->observer<const Ecs::GlobalTransformComponent>()
                .with<const Ecs::RenderableComponent>()
                .or_()
                .with<const Ecs::LightTagComponent>()
                .event(flecs::OnRemove)
                .event(flecs::OnSet);
        }
    };

    using EntityTransformGpuStorage = EntityGpuStorage<EntityGpuStorageTraits_Transform>;
} // namespace Ame::Gfx