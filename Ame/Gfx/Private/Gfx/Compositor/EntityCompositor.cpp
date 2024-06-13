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
        const EcsSystemDesc& ecsDesc) :
        m_SystemHooks(std::make_unique<EcsSystemHooks>(rhiDevice, universe, ecsDesc))
    {
    }

    EntityCompositor::~EntityCompositor() = default;

    //

    void EntityCompositor::DispatchCamera(
        RG::Graph&                       renderGraph,
        const Ecs::Entity&               cameraEntity,
        const Ecs::Component::Camera&    cameraComponent,
        const Ecs::Component::Transform& cameraTransform)
    {
        renderGraph.UpdateFrameStorage(
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

        FetchAndSort(drawData);
        ExecuteAndClear(renderGraph);
    }

    std::vector<Ecs::Entity> EntityCompositor::GetRenderables(
        const Ecs::Entity& cameraEntity) const
    {
        return std::vector<Ecs::Entity>();
    }

    void EntityCompositor::FetchAndSort(
        Signals::Data::DrawCompositorData& drawData)
    {
        m_OnRenderCompose(drawData);
        m_DrawCompositor.Sort();
    }

    void EntityCompositor::ExecuteAndClear(
        RG::Graph& renderGraph)
    {
        renderGraph.Execute();
        m_DrawCompositor.Clear();
    }
} // namespace Ame::Gfx