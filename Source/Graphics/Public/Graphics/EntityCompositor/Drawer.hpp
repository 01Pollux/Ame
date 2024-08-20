#pragma once

#include <Graphics/EntityCompositor/Core.hpp>
#include <Ecs/Query.hpp>

#include <Graphics/EntityCompositor/EntityGpuStorage/DrawInstanceTable.hpp>
#include <Graphics/EntityCompositor/EntityGpuStorage/LightTable.hpp>

namespace Ame::Gfx
{
    class EntityDrawer
    {
    private:
        using RenderableQuery = Ecs::UniqueQuery<
            const Ecs::RenderableComponent,
            const EntityDrawInstance_EcsId>;

        using LightQuery = Ecs::UniqueQuery<
            const EntityLightInstance_EcsId>;

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
        LightQuery           m_LightQuery;
    };
} // namespace Ame::Gfx