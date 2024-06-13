#include <RG/ResourceStorage.hpp>
#include <RG/Resources/CoreResources.hpp>
#include <RG/Resources/Names.hpp>

namespace Ame::RG
{
    void ResourceStorage::ImportCoreResources()
    {
        auto& frameResource = m_Resources[Names::c_FrameResource];
        if (!frameResource) [[unlikely]]
        {
            frameResource.Import(m_CoreResources->GetFrameResource().Borrow());
        }

        frameResource.CreateBufferView(
            Names::c_FrameResourceMainView,
            m_CoreResources->GetFrameResourceViewDesc());
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
    }
} // namespace Ame::RG