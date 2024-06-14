#include <Gfx/Compositor/EntityCompositor.hpp>
#include <Gfx/Ecs/System.hpp>

#include <RG/Graph.hpp>
#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

namespace Ame::Gfx
{
    EntityCompositor::EntityCompositor(
        Rhi::Device&         rhiDevice,
        Ecs::Universe&       universe,
        RG::Graph&           renderGraph,
        const EcsSystemDesc& ecsDesc) :
        m_Graph(renderGraph),
        m_SystemHooks(std::make_unique<EcsSystemHooks>(rhiDevice, universe, ecsDesc))
    {
    }

    EntityCompositor::~EntityCompositor() = default;

    //

    void EntityCompositor::UpdateGraph()
    {
        m_Graph.get().Update();
    }

    void EntityCompositor::RenderGraph(
        const Ecs::Entity&               cameraEntity,
        const Ecs::Component::Camera&    cameraComponent,
        const Ecs::Component::Transform& cameraTransform)
    {
        m_Graph.get().UpdateFrameStorage(
            cameraEntity,
            cameraTransform,
            cameraComponent.GetProjectionMatrix(),
            cameraComponent.GetViewporSize());

        auto renderables = GetRenderables(cameraEntity);

        Signals::Data::DrawCompositorData drawData{
            .Compositor      = DrawCompositorSubmitter(m_DrawCompositor),
            .CameraEntity    = cameraEntity,
            .CameraComponent = cameraComponent,
            .CameraTransform = cameraTransform,
            .Entities        = renderables
        };

        FetchAndSortDrawData(drawData);
        ExecuteAndClearGraph();
    }

    //

    std::vector<Ecs::Entity> EntityCompositor::GetRenderables(
        const Ecs::Entity& cameraEntity) const
    {
        return std::vector<Ecs::Entity>();
    }

    void EntityCompositor::FetchAndSortDrawData(
        Signals::Data::DrawCompositorData& drawData)
    {
        m_OnRenderCompose(drawData);
        m_DrawCompositor.Sort();
    }

    void EntityCompositor::ExecuteAndClearGraph()
    {
        m_Graph.get().Execute();
        m_DrawCompositor.Clear();
    }
} // namespace Ame::Gfx