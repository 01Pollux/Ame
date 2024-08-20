#pragma once

#include <Graphics/EntityCompositor/EntityGpuStorage/EntityGpuStorage.hpp>
#include <Graphics/EntityCompositor/EntityGpuStorage/TransformTable.hpp>

namespace Ame::Gfx
{
    struct EntityDrawInstance_EcsId
    {
        uint32_t Id = std::numeric_limits<uint32_t>::max();
    };

    struct EntityGpuStorageTraits_DrawInstance
    {
        static constexpr const char* name = "DrawInstanceTable";

        using id_container_type = EntityDrawInstance_EcsId;
        using instance_type     = EntityDrawInstance;

        static void update(const Ecs::Entity& entity, instance_type& instance)
        {
            auto transformId     = entity->get<EntityTransform_EcsId>()->Id;
            instance.TransformId = transformId;
            // TODO: Add bounding box/sphere optional
            // TODO: Add instance code
        }

        static Ecs::ObserverBuilder<> observer_create(Ecs::WorldRef world)
        {
            return world
                ->observer<>()
                .with<Ecs::RenderableComponent>()
                .in()
                .and_()
                .with<EntityTransform_EcsId>()
                .in()
                .event(flecs::OnRemove)
                .event(flecs::OnSet);
        }
    };

    using EntityDrawInstanceGpuStorage = EntityGpuStorage<EntityGpuStorageTraits_DrawInstance>;
} // namespace Ame::Gfx