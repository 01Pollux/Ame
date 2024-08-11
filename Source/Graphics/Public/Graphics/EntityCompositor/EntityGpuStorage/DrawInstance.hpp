#pragma once

#include <Graphics/EntityCompositor/EntityGpuStorage/EntityGpuStorage.hpp>

namespace Ame::Gfx
{
    struct EntityDrawInstance_EcsId
    {
        uint32_t Id = std::numeric_limits<uint32_t>::max();
    };

    struct EntityGpuStorageTraits_DrawInstance
    {
        static constexpr const char* name = "DrawInstance";

        using id_container_type = EntityDrawInstance_EcsId;
        using instance_type     = EntityDrawInstance;

        static void update(const Ecs::Entity& entity, instance_type& instance)
        {
            auto  renderable     = entity->get<Ecs::RenderableComponent>()->Object;
            auto& renderableDesc = renderable->GetRenderableDesc();

            // TODO: Add bounding box/sphere optional
            // TODO: Add instance code
            // TODO: Add instance transform
            // TODO: draw type

            instance.VertexOffset = renderableDesc.Vertices.Offset;
            instance.IndexOffset  = renderableDesc.Indices.Offset;
            instance.IndexCount   = renderableDesc.Indices.Count;
        }

        static Ecs::ObserverBuilder<> observer_create(Ecs::WorldRef world)
        {
            return world
                ->observer<>()
                .with<Ecs::RenderableComponent>()
                .event(flecs::OnRemove)
                .event(flecs::OnSet);
        }
    };

    using EntityDrawInstanceGpuStorage = EntityGpuStorage<EntityGpuStorageTraits_DrawInstance>;
} // namespace Ame::Gfx