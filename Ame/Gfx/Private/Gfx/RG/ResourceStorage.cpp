#include <Gfx/RG/ResourceStorage.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>
#include <Gfx/RG/Resources/Names.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    ResourceStorage::ResourceStorage(
        Rhi::Device&   rhiDevice,
        Ecs::Universe& universe) :
        m_Device(rhiDevice),
        m_CoreResources(std::make_unique<CoreResources>(rhiDevice, universe))
    {
        UpdateCoreResources();
    }

    ResourceStorage::~ResourceStorage() = default;

    //

    Rhi::Device& ResourceStorage::GetDevice() const
    {
        return m_Device.get();
    }

    //

    bool ResourceStorage::ContainsResource(
        const ResourceId& id) const
    {
        return m_Resources.contains(id);
    }

    bool ResourceStorage::ContainsResourceView(
        const ResourceViewId& viewId)
    {
        auto iter = m_Resources.find(viewId.GetResource());
        return iter != m_Resources.end() ? iter->second.ContainsView(viewId) : false;
    }

    //

    EntityStore ResourceStorage::GetEntityStore() const
    {
        return EntityStore(m_CoreResources->GetEntities(), m_CoreResources->GetEntitiesCount());
    }

    //

    const ResourceHandle* ResourceStorage::GetResource(
        const ResourceId& id) const
    {
        auto iter = m_Resources.find(id);
        return iter != m_Resources.end() ? &iter->second : nullptr;
    }

    ResourceHandle& ResourceStorage::GetResourceMut(
        const ResourceId& id)
    {
        CheckLockState(false);
        return m_Resources.at(id);
    }

    ResourceViewDesc& ResourceStorage::GetResourceViewDescMut(
        const ResourceViewId& viewId)
    {
        CheckLockState(false);

        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.GetViewDescMut(viewId);
    }

    const ResourceViewDesc& ResourceStorage::GetResourceViewDesc(
        const ResourceViewId& viewId) const
    {
        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.GetViewDesc(viewId);
    }

    const Rhi::ResourceView& ResourceStorage::GetResourceViewHandle(
        const ResourceViewId& viewId) const
    {
        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.GetViewHandle(viewId);
    }

    //

    void ResourceStorage::DeclareBuffer(
        const ResourceId&      id,
        const Rhi::BufferDesc& Desc)
    {
        CheckLockState(false);
        m_Resources[id].SetDynamic(id, Desc);
    }

    void ResourceStorage::DeclareTexture(
        const ResourceId&       id,
        const Rhi::TextureDesc& Desc)
    {
        CheckLockState(false);
        m_Resources[id].SetDynamic(id, Desc);
    }

    //

    void ResourceStorage::ImportBuffer(
        const ResourceId& id,
        Rhi::Buffer       buffer)
    {
        CheckLockState(false);
        m_Resources[id].Import(std::move(buffer));
    }

    void ResourceStorage::ImportTexture(
        const ResourceId& id,
        Rhi::Texture      texture)
    {
        CheckLockState(false);
        m_Resources[id].Import(std::move(texture));
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
        const ResourceViewId& viewId,
        ResourceViewDesc      desc)
    {
        CheckLockState(false);

        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.CreateBufferView(viewId, std::move(desc));
    }

    Rhi::TextureDesc& ResourceStorage::DeclareTextureView(
        const ResourceViewId& viewId,
        ResourceViewDesc      desc)
    {
        CheckLockState(false);

        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.CreateTextureView(viewId, std::move(desc));
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
        bool locked) const
    {
#ifndef AME_DIST
        AME_LOG_ASSERT(Log::Gfx(), m_Locked == locked, "ResourceStorage is{} locked", locked ? "" : "n't");
#endif
    }
} // namespace Ame::Gfx::RG