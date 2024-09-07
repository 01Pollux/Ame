#include <Graphics/EntityCompositor/Drawer.hpp>
#include <Graphics/EntityCompositor/Collector.hpp>
#include <Graphics/EntityCompositor/Storage.hpp>

namespace Ame::Gfx
{
    EntityDrawer::EntityDrawer(EntityCollector& collector) : m_Collector(collector)
    {
        auto world        = collector.GetWorld();
        m_RenderableQuery = world->CreateQuery<const Ecs::RenderableComponent, const EntityDrawInstance_EcsId>()
                                .with<Ecs::ActiveSceneEntityTag>()
                                .cached()
                                .build();
        m_LightQuery = world->CreateQuery<const EntityLightInstance_EcsId>()
                           .with<const Ecs::LightTagComponent>()
                           .with<Ecs::ActiveSceneEntityTag>()
                           .cached()
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
        if (m_LightQuery->changed())
        {
            std::vector<uint32_t> lightIds;
            lightIds.push_back(0); // Light count
            m_LightQuery->run(
                [this, &lightIds](Ecs::Iterator& iter)
                {
                    while (iter.next())
                    {
                        auto instanceIds = iter.field<const EntityLightInstance_EcsId>(0);
                        lightIds.reserve(lightIds.size() + iter.count());
                        for (size_t i : iter)
                        {
                            lightIds.push_back(instanceIds[i].Id);
                        }
                    }
                });
            lightIds[0] = Rhi::Count32(lightIds) - 1;
            m_Collector.get().UpdateLights(lightIds);
        }
    }

    void EntityDrawer::Draw(Rg::Graph& cameraGraph, const Math::Camera& camera,
                            const Math::TransformMatrix& cameraTransform)
    {
        Math::Matrix4x4 cameraView = Math::Util::XMMatrixLookToLH(
            cameraTransform.GetPosition(), cameraTransform.GetLookDir(), cameraTransform.GetUpDir());

        CameraFrameDataUpdateDesc frameDesc{
            .World      = cameraTransform.ToMat4x4(),
            .View       = cameraView,
            .Projection = camera.GetProjectionMatrix(),
            .Viewport   = camera.GetViewporSize(),
            .NearPlane  = camera.GetViewport().NearPlane,
            .FarPlane   = camera.GetViewport().FarPlane,
            .CameraMask = 0xFFFF'FFFF // TODO: Add camera mask
        };

        //

        m_Collector.get().Sort(cameraTransform);
        m_Collector.get().Upload(cameraGraph, frameDesc);

        //

        cameraGraph.Execute();

        //

        m_Collector.get().Reset();
    }
} // namespace Ame::Gfx
