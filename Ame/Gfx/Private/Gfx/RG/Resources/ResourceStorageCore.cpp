#include <Gfx/RG/ResourceStorage.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>
#include <Gfx/RG/Resources/Names.hpp>

namespace Ame::Gfx::RG
{
    void ResourceStorage::ResetCameraStorage()
    {
        m_CoreResources->ResetCameraStorage();
    }

    void ResourceStorage::UpdateCoreResources()
    {
        auto& FrameResource = m_Resources[Names::FrameResource];
        if (!FrameResource) [[unlikely]]
        {
            FrameResource.Import(m_CoreResources->GetFrameResource());
        }

        FrameResource.CreateBufferView(
            Names::FrameResourceMainView,
            m_CoreResources->GetFrameResourceViewDesc());

        //

        auto& TransformTable  = m_Resources[Names::TransformsTable];
        auto& TransformBuffer = m_CoreResources->GetTransformBuffer();
        TransformBuffer.Flush();
        if (!TransformTable || *TransformTable.AsBuffer() != TransformBuffer.GetBuffer()) [[unlikely]]
        {
            TransformTable.Import(m_CoreResources->GetTransformBuffer().GetBuffer());
        }

        //

        auto& RenderInstanceTable = m_Resources[Names::RenderInstancesTable];
        RenderInstanceTable.Import(Rhi::Buffer(nullptr));
    }

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

        //

        auto& RenderInstanceTable = m_Resources[Names::RenderInstancesTable];
        if (m_CoreResources->GetEntitiesCount())
        {
            auto& CurRenderInstanceTable = m_CoreResources->GetInstancesTableBuffer();
            CurRenderInstanceTable.Flush();
            RenderInstanceTable.Import(CurRenderInstanceTable.GetBuffer());
        }
    }
} // namespace Ame::Gfx::RG