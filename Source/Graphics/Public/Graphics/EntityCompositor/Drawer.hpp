#pragma once

#include <Graphics/EntityCompositor/Core.hpp>
#include <Ecs/Query.hpp>

#include <Graphics/EntityCompositor/EntityGpuStorage/DrawInstanceTable.hpp>

namespace Ame::Gfx
{
    class EntityDrawer
    {
    private:
        using RenderableQuery = Ecs::UniqueQuery<
            const Ecs::RenderableComponent,
            const EntityDrawInstance_EcsId>;

    public:
        EntityDrawer(
            EntityCollector& collector);

        void Update();
        void Draw(
            Rg::Graph&                   cameraGraph,
            const Math::Camera&          camera,
            const Math::TransformMatrix& cameraTransform);

    private:
        Ref<EntityCollector> m_Collector;
        RenderableQuery      m_RenderableQuery;
    };
} // namespace Ame::Gfx