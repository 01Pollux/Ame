#include <Gfx/RG/ResourceStorage.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>
#include <Gfx/RG/Resources/Names.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    ResourceStorage::ResourceStorage(
        Rhi::Device&   Device,
        Ecs::Universe& Universe) :
        m_Device(Device),
        m_CoreResources(std::make_unique<CoreResources>(Device, Universe))
    {
    }

    ResourceStorage::~ResourceStorage() = default;

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
        return GetResource(Names::FrameResource);
    }

    const Rhi::ResourceView& ResourceStorage::GetFrameResourceHandle() const
    {
        return GetResourceViewHandle(Names::FrameResourceMainView);
    }

    const FrameResourceCPU& ResourceStorage::GetFrameResourceData() const
    {
        return m_CoreResources->GetFrameResourceData();
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
        CheckLockState(false);
        return m_Resources.at(Id);
    }

    ResourceViewDesc& ResourceStorage::GetResourceViewDescMut(
        const ResourceViewId& ViewId)
    {
        CheckLockState(false);

        auto Iter = m_Resources.find(ViewId.GetResource());
        AME_LOG_ASSERT(Log::Renderer(), Iter != m_Resources.end(), "Resource doesn't exists");
        return Iter->second.GetViewDescMut(ViewId);
    }

    const ResourceViewDesc& ResourceStorage::GetResourceViewDesc(
        const ResourceViewId& ViewId) const
    {
        auto Iter = m_Resources.find(ViewId.GetResource());
        AME_LOG_ASSERT(Log::Renderer(), Iter != m_Resources.end(), "Resource doesn't exists");
        return Iter->second.GetViewDesc(ViewId);
    }

    const Rhi::ResourceView& ResourceStorage::GetResourceViewHandle(
        const ResourceViewId& ViewId) const
    {
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