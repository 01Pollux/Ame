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
        const EcsWorldResourcesDesc& ecsDesc) :
        m_Graph(renderGraph),
        m_EcsResources(std::make_unique<EcsWorldResources>(rhiDevice, universe, ecsDesc))
    {
    }

    EntityCompositor::~EntityCompositor() = default;

    //

    std::span<const DrawInstanceOrder> EntityCompositor::GetDrawInstances(
        DrawInstanceType type) const noexcept
    {
        return m_DrawCompositor.GetInstances(type);
    }

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

        GetRenderables(cameraEntity);
        FetchAndSortDrawData(cameraEntity, cameraComponent, cameraTransform);
        ExecuteAndClearGraph();
    }

    //

    void EntityCompositor::FlushAndUploadResourcesToGraph()
    {
        auto& transformBuffer = m_EcsResources->GetTransformBuffer();
        auto& aabbBuffer      = m_EcsResources->GetAABBBuffer();
        auto& instanceBuffer  = m_EcsResources->GetInstanceBuffer();

        transformBuffer.Flush();
        aabbBuffer.Flush();
        instanceBuffer.Flush();

        //

        auto& resourceStorage = m_Graph.get().GetResourceStorage();

        resourceStorage.ImportBuffer(RG::Names::c_TransformsTable, transformBuffer.GetBuffer().Borrow());
        resourceStorage.ImportBuffer(RG::Names::c_AABBTable, aabbBuffer.GetBuffer().Borrow());
        resourceStorage.ImportBuffer(RG::Names::c_InstanceTable, instanceBuffer.GetBuffer().Borrow());
    }

    //

    void EntityCompositor::GetRenderables(
        const Ecs::Entity& cameraEntity)
    {
        m_DrawInfos.clear();

        auto&    renderRule = m_EcsResources->GetCameraRenderRule();
        uint32_t cameraVar  = renderRule->find_var("Camera");

        renderRule
            ->iter()
            .set_var(cameraVar, *cameraEntity)
            .iter(
                [this](
                    Ecs::Iterator&                        iter,
                    const RenderInstance::GpuId*          instanceIds,
                    const Ecs::Component::Transform*      transforms,
                    const Ecs::Component::BaseRenderable* renderables)
                {
                    m_DrawInfos.reserve(m_DrawInfos.size() + iter.count());
                    for (auto i : iter)
                    {
                        m_DrawInfos.emplace_back(iter.entity(i), instanceIds[i].Id, renderables[i], transforms[i]);
                    }
                });
    }

    void EntityCompositor::FetchAndSortDrawData(
        const Ecs::Entity&               cameraEntity,
        const Ecs::Component::Camera&    cameraComponent,
        const Ecs::Component::Transform& cameraTransform)
    {
        Signals::Data::DrawCompositorData drawData{
            .Compositor      = DrawCompositorSubmitter(m_DrawCompositor),
            .CameraEntity    = cameraEntity,
            .CameraComponent = cameraComponent,
            .CameraTransform = cameraTransform,
            .Entities        = m_DrawInfos
        };
        m_OnRenderCompose(drawData);
        m_DrawCompositor.Sort();
    }

    void EntityCompositor::ExecuteAndClearGraph()
    {
        m_Graph.get().Execute();
        m_DrawCompositor.Clear();
    }
} // namespace Ame::Gfx