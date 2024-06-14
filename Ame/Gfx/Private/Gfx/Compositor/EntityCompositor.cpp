#include <Gfx/Compositor/EntityCompositor.hpp>
#include <Gfx/Ecs/World.hpp>

#include <RG/Graph.hpp>
#include <Gfx/RenderGraph/Resources/Names.hpp>

#include <Ecs/Component/Math/Transform.hpp>
#include <Ecs/Component/Viewport/Camera.hpp>

namespace Ame::Gfx
{
    EntityCompositor::EntityCompositor(
        Rhi::Device&                 rhiDevice,
        Ecs::Universe&               universe,
        RG::Graph&                   renderGraph,
        Extensions::WorldOctTreeBox& worldOctTreeBox,
        const EcsWorldResourcesDesc& ecsDesc) :
        m_Graph(renderGraph),
        m_WorldOctTreeBox(worldOctTreeBox),
        m_EcsResources(std::make_unique<EcsWorldResources>(rhiDevice, universe, ecsDesc))
    {
    }

    EntityCompositor::~EntityCompositor() = default;

    //

    void EntityCompositor::UpdateGraph()
    {
        FlushAndUploadResourcesToGraph();
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

        auto renderables = GetRenderables(cameraComponent);

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

    void EntityCompositor::FlushAndUploadResourcesToGraph()
    {
        auto& transformBuffer = m_EcsResources->GetTransformBuffer();
        auto& aabbBuffer      = m_EcsResources->GetAABBBuffer();

        transformBuffer.Flush();
        aabbBuffer.Flush();

        //

        auto& resourceStorage = m_Graph.get().GetResourceStorage();

        resourceStorage.ImportBuffer(RG::Names::c_TransformsTable, transformBuffer.GetBuffer().Borrow());
        resourceStorage.ImportBuffer(RG::Names::c_AABBTable, aabbBuffer.GetBuffer().Borrow());
    }

    //

    std::vector<EntityDrawInfo> EntityCompositor::GetRenderables(
        const Ecs::Component::Camera& cameraComponent) const
    {
        Geometry::FrustumPlanes frustumPlanes(Geometry::Frustum(cameraComponent.GetProjectionMatrix()));

        auto transformToDrawInfo = [&cameraComponent](const Ecs::Entity& entity) -> EntityDrawInfo
        {
            auto renderable        = entity.TryGetComponent<Ecs::Component::BaseRenderable>();
            bool isValidRenderable = renderable && ((renderable->CameraMask & cameraComponent.ViewMask) != 0);
            return isValidRenderable ? EntityDrawInfo{ entity, renderable, entity.TryGetComponent<Ecs::Component::Transform>() } : EntityDrawInfo{};
        };

        auto filterDrawInfo = [](const EntityDrawInfo& drawInfo)
        {
            return drawInfo.Renderable != nullptr;
        };

        return m_WorldOctTreeBox.get().FrustumCull(frustumPlanes) |
               std::views::transform(transformToDrawInfo) |
               std::views::filter(filterDrawInfo) |
               std::ranges::to<std::vector>();
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