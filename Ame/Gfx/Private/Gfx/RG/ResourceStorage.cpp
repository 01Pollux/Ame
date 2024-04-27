#include <Gfx/RG/ResourceStorage.hpp>
#include <Math/Common.hpp>
#include <Rhi/Device/Device.hpp>
#include <Rhi/Util/ResourceSize.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    ResourceStorage::ResourceStorage(
        Rhi::Device& Device) :
        m_Device(Device)
    {
        AllocateFrameResource();
    }

    //

    Rhi::Device& ResourceStorage::GetDevice() const
    {
        return m_Device.get();
    }

    //

    bool ResourceStorage::ContainsResource(
        const ResourceId& Id) const
    {
        return m_Resources.contains(Id);
    }

    bool ResourceStorage::ContainsResourceView(
        const ResourceViewId& ViewId)
    {
        auto Iter = m_Resources.find(ViewId.GetResource());
        return Iter != m_Resources.end() ? Iter->second.ContainsView(ViewId) : false;
    }

    //

    const ResourceHandle& ResourceStorage::GetFrameResource() const
    {
        return GetResource(ResourceId::FrameResource);
    }

    const Rhi::ResourceView& ResourceStorage::GetFrameResourceHandle() const
    {
        return GetResourceViewHandle(ResourceId::FrameResource("Main"));
    }

    const FrameResource& ResourceStorage::GetFrameResourceData() const
    {
        return m_FrameResource;
    }

    //

    const ResourceHandle& ResourceStorage::GetResource(
        const ResourceId& Id) const
    {
        return m_Resources.at(Id);
    }

    ResourceHandle& ResourceStorage::GetResourceMut(
        const ResourceId& Id)
    {
        return m_Resources.at(Id);
    }

    ResourceViewDesc& ResourceStorage::GetResourceViewMut(
        const ResourceViewId& ViewId)
    {
        CheckLockState(false);

        auto Iter = m_Resources.find(ViewId.GetResource());
        AME_LOG_ASSERT(Log::Renderer(), Iter != m_Resources.end(), "Resource doesn't exists");
        return Iter->second.GetViewMut(ViewId);
    }

    const ResourceViewDesc& ResourceStorage::GetResourceView(
        const ResourceViewId& ViewId) const
    {
        CheckLockState(false);

        auto Iter = m_Resources.find(ViewId.GetResource());
        AME_LOG_ASSERT(Log::Renderer(), Iter != m_Resources.end(), "Resource doesn't exists");
        return Iter->second.GetView(ViewId);
    }

    const Rhi::ResourceView& ResourceStorage::GetResourceViewHandle(
        const ResourceViewId& ViewId) const
    {
        CheckLockState(false);

        auto Iter = m_Resources.find(ViewId.GetResource());
        AME_LOG_ASSERT(Log::Renderer(), Iter != m_Resources.end(), "Resource doesn't exists");
        return Iter->second.GetViewHandle(ViewId);
    }

    //

    void ResourceStorage::DeclareBuffer(
        const ResourceId&      Id,
        const Rhi::BufferDesc& Desc)
    {
        CheckLockState(false);
        m_Resources[Id].SetDynamic(Desc);
    }

    void ResourceStorage::DeclareTexture(
        const ResourceId&       Id,
        const Rhi::TextureDesc& Desc)
    {
        CheckLockState(false);
        m_Resources[Id].SetDynamic(Desc);
    }

    //

    void ResourceStorage::ImportBuffer(
        const ResourceId&  Id,
        const Rhi::Buffer& Buffer)
    {
        CheckLockState(false);

        AME_LOG_ASSERT(Log::Renderer(), Buffer, "Buffer is nullptr");
        m_Resources[Id].Import(Buffer);
    }

    void ResourceStorage::ImportTexture(
        const ResourceId&   Id,
        const Rhi::Texture& Texture)
    {
        CheckLockState(false);

        AME_LOG_ASSERT(Log::Renderer(), Texture, "Texture is nullptr");
        m_Resources[Id].Import(Texture);
    }

    //

    void ResourceStorage::UpdateResources()
    {
        CheckLockState(false);

        for (auto& Handle : m_Resources)
        {
            Handle.second.Reallocate(m_Device.get());
        }
    }

    Rhi::BufferDesc& ResourceStorage::DeclareBufferView(
        const ResourceViewId& ViewId,
        ResourceViewDesc      ViewDesc)
    {
        CheckLockState(false);

        auto Iter = m_Resources.find(ViewId.GetResource());
        AME_LOG_ASSERT(Log::Renderer(), Iter != m_Resources.end(), "Resource doesn't exists");
        return Iter->second.CreateBufferView(ViewId, std::move(ViewDesc));
    }

    Rhi::TextureDesc& ResourceStorage::DeclareTextureView(
        const ResourceViewId& ViewId,
        ResourceViewDesc      ViewDesc)
    {
        CheckLockState(false);

        auto Iter = m_Resources.find(ViewId.GetResource());
        AME_LOG_ASSERT(Log::Renderer(), Iter != m_Resources.end(), "Resource doesn't exists");
        return Iter->second.CreateTextureView(ViewId, std::move(ViewDesc));
    }

    //

    void ResourceStorage::UpdateFrameResource(
        float                        EngineTime,
        float                        GameTime,
        float                        DeltaTime,
        const Math::TransformMatrix& Transform,
        const Math::Matrix4x4&       Projection,
        const Math::Vector2&         Viewport)
    {
        CheckLockState(false);

        auto View = glm::lookAt(
            Transform.GetPosition(),
            Transform.GetPosition() + Transform.GetLookDir(),
            Transform.GetUpDir());

        m_FrameResource.World = Transform.ToMat4x4Transposed();

        m_FrameResource.View           = glm::transpose(View);
        m_FrameResource.Projection     = glm::transpose(Projection);
        m_FrameResource.ViewProjection = m_FrameResource.View * m_FrameResource.Projection;

        m_FrameResource.ViewInverse           = glm::inverse(m_FrameResource.View);
        m_FrameResource.ProjectionInverse     = glm::inverse(m_FrameResource.Projection);
        m_FrameResource.ViewProjectionInverse = glm::inverse(m_FrameResource.ViewProjection);

        m_FrameResource.Viewport = Viewport;

        m_FrameResource.EngineTime = EngineTime;
        m_FrameResource.GameTime   = GameTime;
        m_FrameResource.DeltaTime  = DeltaTime;

        auto&   DeviceDesc = m_Device.get().GetDesc();
        uint8_t FrameIndex = m_Device.get().GetFrameIndex();
        size_t  Offset     = Rhi::GetConstantBufferSize(DeviceDesc, sizeof(m_FrameResource), FrameIndex);
        std::memcpy(m_FrameResourceBuffer.GetPtr(Offset), &m_FrameResource, sizeof(m_FrameResource));

        auto& Resource = m_Resources[ResourceId::FrameResource];
        Resource.Import(m_FrameResourceBuffer);

        Resource.CreateBufferView(
            ResourceId::FrameResource("Main"),
            Rhi::BufferViewDesc{ .Type = Rhi::BufferViewType::ConstantBuffer });
    }

    void ResourceStorage::AllocateFrameResource()
    {
        auto&   DeviceDesc = m_Device.get().GetDesc();
        uint8_t FrameCount = m_Device.get().GetFrameCountInFlight();
        size_t  BufferSize = Rhi::GetConstantBufferSize(DeviceDesc, sizeof(m_FrameResource), FrameCount);

        m_FrameResourceBuffer = Rhi::Buffer(m_Device.get(), Rhi::MemoryLocation::HOST_UPLOAD, { .size = BufferSize, .usageMask = Rhi::BufferUsageBits::CONSTANT_BUFFER });
    }

    void ResourceStorage::Lock()
    {
#ifndef AME_DIST
        m_Locked = true;
#endif
    }

    void ResourceStorage::Unlock()
    {
#ifndef AME_DIST
        m_Locked = false;
#endif
    }

    void ResourceStorage::CheckLockState(
        bool Locked) const
    {
#ifndef AME_DIST
        AME_LOG_ASSERT(Log::Renderer(), m_Locked == Locked, "ResourceStorage is{} locked", Locked ? "" : "n't");
#endif
    }
} // namespace Ame::Gfx::RG