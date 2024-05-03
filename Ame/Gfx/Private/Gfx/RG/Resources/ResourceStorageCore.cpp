#include <Gfx/RG/ResourceStorage.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>
#include <Gfx/RG/Resources/Names.hpp>

namespace Ame::Gfx::RG
{
    void ResourceStorage::UpdateCoreResources()
    {
        auto& FrameResource = m_Resources[Names::FrameResource];
        if (!FrameResource)
        {
            FrameResource.Import(m_CoreResources->GetFrameResource());
            FrameResource.CreateBufferView(
                Names::FrameResourceMain,
                Rhi::BufferViewDesc{ .Type = Rhi::BufferViewType::ConstantBuffer });
        }
    }

    //

    void ResourceStorage::UpdateFrameResource(
        float                        EngineTime,
        float                        GameTime,
        float                        DeltaTime,
        const Ecs::Entity&           CameraEntity,
        const Math::TransformMatrix& Transform,
        const Math::Matrix4x4&       Projection,
        const Math::Vector2&         Viewport)
    {
        CheckLockState(false);
        m_CoreResources->UpdateFrameResource(EngineTime, GameTime, DeltaTime, CameraEntity, Transform, Projection, Viewport);
        m_CoreResources->CollectEntities();
    }
} // namespace Ame::Gfx::RG