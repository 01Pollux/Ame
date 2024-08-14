#include <Graphics/EntityCompositor/Drawer.hpp>
#include <Graphics/EntityCompositor/Collector.hpp>
#include <Graphics/EntityCompositor/Storage.hpp>

namespace Ame::Gfx
{
    EntityDrawer::EntityDrawer(
        EntityCollector& collector) :
        m_Collector(collector)
    {
        auto world = collector.GetWorld();
        m_RenderableQuery =
            world->CreateQuery<
                     const Ecs::RenderableComponent,
                     const EntityDrawInstance_EcsId>()
                .build();
    }

    void EntityDrawer::Update()
    {
        m_Collector.get().ResetCommands();
        m_RenderableQuery->run(
            [this](Ecs::Iterator& iter)
            {
                while (iter.next())
                {
                    auto renderables = iter.field<const Ecs::RenderableComponent>(0);
                    auto instanceIds = iter.field<const EntityDrawInstance_EcsId>(1);

                    for (size_t i : iter)
                    {
                        m_Collector.get().AddEntity(DrawInstanceType::Opaque, renderables[i].Object, instanceIds[i].Id);
                    }
                }
            });
    }

    void EntityDrawer::Draw(
        RG::Graph&                   cameraGraph,
        const Ecs::Entity&           cameraEntity,
        const Math::Camera&          camera,
        const Math::TransformMatrix& cameraTransform)
    {
        Math::Matrix4x4 cameraView = Math::Util::XMMatrixLookToLH(
            cameraTransform.GetPosition(),
            cameraTransform.GetLookDir(),
            cameraTransform.GetUpDir());

        CameraFrameDataUpdateDesc frameData{
            .WorldTransposed      = cameraTransform.ToMat4x4(),
            .ViewTransposed       = cameraView,
            .ProjectionTransposed = camera.GetProjectionMatrix(),
            .Viewport             = camera.GetViewporSize(),
            .CameraMask           = 0xFFFF'FFFF // TODO: Add camera mask
        };

        //

        m_Collector.get().Sort(cameraTransform);
        // TODO: m_Collector.get().Collapse(); // Group all draw instances with same index and vertex
        m_Collector.get().Upload(cameraGraph, frameData);

        //

        cameraGraph.Execute();

        //

        m_Collector.get().Reset();
    }
} // namespace Ame::Gfx
