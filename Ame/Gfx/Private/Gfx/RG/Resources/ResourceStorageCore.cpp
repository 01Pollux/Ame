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
        auto& frameResource = m_Resources[Names::c_FrameResource];
        if (!frameResource) [[unlikely]]
        {
            frameResource.Import(m_CoreResources->GetFrameResource().Borrow());
        }

        frameResource.CreateBufferView(
            Names::c_FrameResourceMainView,
            m_CoreResources->GetFrameResourceViewDesc());

        //

        auto& transformTable  = m_Resources[Names::c_TransformsTable];
        auto& transformBuffer = m_CoreResources->GetTransformBuffer();
        transformBuffer.Flush();

        if (!transformTable ||
            (transformBuffer.GetBuffer() != *transformTable.AsBuffer())) [[unlikely]]
        {
            transformTable.Import(transformBuffer.GetBuffer().Borrow());
        }

        //

        auto& renderInstanceTable = m_Resources[Names::c_RenderInstancesTable];
        renderInstanceTable.Import(Rhi::Buffer(nullptr));
    }

    void ResourceStorage::UpdateFrameResource(
        float                        engineTime,
        float                        gameTime,
        float                        deltaTime,
        const Ecs::Entity&           cameraEntity,
        const Math::TransformMatrix& transform,
        const Math::Matrix4x4&       projection,
        const Math::Vector2&         viewport)
    {
        CheckLockState(false);
        m_CoreResources->UpdateFrameResource(
            engineTime,
            gameTime,
            deltaTime,
            cameraEntity,
            transform,
            projection,
            viewport);

        //

        m_CoreResources->CollectEntities();
        if (m_CoreResources->GetEntitiesCount())
        {
            auto& renderInstanceTable    = m_Resources[Names::c_RenderInstancesTable];
            auto& curRenderInstanceTable = m_CoreResources->GetInstancesTableBuffer();
            renderInstanceTable.Import(curRenderInstanceTable.GetBuffer().Borrow());
        }
    }
} // namespace Ame::Gfx::RG