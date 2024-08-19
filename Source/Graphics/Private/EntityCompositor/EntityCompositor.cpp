#include <Graphics/EntityCompositor/EntityCompositor.hpp>

namespace Ame::Gfx
{
    EntityCompositor::EntityCompositor(
        Rhi::IRhiDevice* rhiDevice,
        Ecs::World*      world) :
        m_Storage(rhiDevice, world),
        m_Collector(m_Storage),
        m_Drawer(m_Collector)
    {
    }

    void EntityCompositor::Update()
    {
        m_Drawer.Update();
    }

    void EntityCompositor::RenderGraph(
        Rg::Graph&                   renderGraph,
        const Math::Camera&          camera,
        const Math::TransformMatrix& transform)
    {
        m_Drawer.Draw(renderGraph, camera, transform);
    }
} // namespace Ame::Gfx