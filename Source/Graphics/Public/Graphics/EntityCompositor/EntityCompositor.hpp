#pragma once

#include <Graphics/EntityCompositor/Storage.hpp>
#include <Graphics/EntityCompositor/Collector.hpp>
#include <Graphics/EntityCompositor/Drawer.hpp>

namespace Ame::Gfx
{
    class EntityCompositor
    {
    public:
        EntityCompositor(
            Rhi::IRhiDevice* rhiDevice,
            Ecs::World*      world);

        void Update();
        void RenderGraph(
            RG::Graph&                   renderGraph,
            const Ecs::Entity&           entity,
            const Math::Camera&          camera,
            const Math::TransformMatrix& transform);

    private:
        EntityStorage   m_Storage;
        EntityCollector m_Collector;
        EntityDrawer    m_Drawer;
    };
} // namespace Ame::Gfx