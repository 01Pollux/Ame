#include <Graphics/EntityCompositor/Drawer.hpp>
#include <Graphics/EntityCompositor/Collector.hpp>
#include <Graphics/EntityCompositor/Storage.hpp>

namespace Ame::Gfx
{
    void EntityDrawer::Update()
    {
        m_Collector.get().Reset();
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
            .WorldTransposed      = cameraTransform.ToMat4x4Transposed(),
            .ViewTransposed       = cameraView.GetTranspose(),
            .ProjectionTransposed = camera.GetProjectionMatrix().GetTranspose(),
            .Viewport             = camera.GetViewporSize(),
            .CameraMask           = 0xFFFF'FFFF // TODO: Add camera mask
        };

        //

        m_Collector.get().Sort(cameraTransform);
        m_Collector.get().Upload(cameraGraph, frameData);

        //

        cameraGraph.Execute(m_Collector.get().GetRenderDevice());
    }
} // namespace Ame::Gfx
