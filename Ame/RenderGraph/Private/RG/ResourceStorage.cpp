#include <RG/ResourceStorage.hpp>
#include <RG/Resources/CoreResources.hpp>
#include <RG/Resources/Names.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/WindowManager.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    ResourceStorage::ResourceStorage(
        Rhi::Device& rhiDevice) :
        m_Device(rhiDevice),
        m_StateTracker(rhiDevice.GetDesc()),
        m_ResourceCache(rhiDevice),
        m_CoreResources(std::make_unique<CoreResources>(rhiDevice))
    {
        ImportCoreResources();
    }

    ResourceStorage::~ResourceStorage()
    {
        for (auto& [_, resource] : m_Resources)
        {
            resource.Release();
        }
    }

    //

    ResourceStateTracker& ResourceStorage::GetStateTracker() noexcept
    {
        return m_StateTracker;
    }

    //

    Rhi::Device& ResourceStorage::GetDevice() const
    {
        return m_Device.get();
    }

    Rhi::ResourceFormat ResourceStorage::GetBackbufferFormat() const
    {
        auto& windowManager = GetDevice().GetWindowManager();
        return windowManager.GetBackbufferFormat();
    }

    const Rhi::TextureDesc& ResourceStorage::GetBackbufferDesc() const
    {
        auto& windowManager = GetDevice().GetWindowManager();
        auto& backbuffer    = windowManager.GetBackbuffer().get();
        return backbuffer.Resource.GetDesc();
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
        if (iter == m_Resources.end())
        {
            return false;
        }

        bool contains = false;
        std::visit(
            VariantVisitor{
                [](std::monostate) {},
                [&](const auto& resource)
                { contains = resource.Views.contains(viewId); } },
            iter->second.Get());
        return contains;
    }

    //

    const ResourceHandle* ResourceStorage::GetResource(
        const ResourceId& id) const
    {
        auto iter = m_Resources.find(id);
        return iter != m_Resources.end() ? &iter->second : nullptr;
    }

    ResourceHandle* ResourceStorage::GetResourceMut(
        const ResourceId& id)
    {
        CheckLockState(false);
        auto iter = m_Resources.find(id);
        return iter != m_Resources.end() ? &iter->second : nullptr;
    }

    const Rhi::ResourceView* ResourceStorage::GetResourceViewHandle(
        const ResourceViewId& viewId) const
    {
        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");

        const Rhi::ResourceView* view = nullptr;
        std::visit(
            VariantVisitor{
                [](std::monostate) {},
                [&](const auto& resource)
                { view = &resource.Views.at(viewId).View; } },
            iter->second.Get());

        return view;
    }

    //

    void ResourceStorage::DeclareResource(
        const ResourceId& id,
        RhiResource       resource)
    {
        CheckLockState(false);
        AME_LOG_ASSERT(Log::Gfx(), m_Resources.contains(id), "Resource already exists");
        m_Resources[id].SetDynamic(id, std::move(resource));
    }

    //

    void ResourceStorage::ImportBuffer(
        const ResourceId&   id,
        Rhi::MemoryLocation location,
        Rhi::Buffer         buffer,
        Rhi::AccessStage    initialState)
    {
        CheckLockState(false);

        auto& resource          = m_Resources[id];
        auto& resourceAllocator = m_Device.get().GetResourceAllocator();

        resource.Release(m_StateTracker);
        resource.Import(BufferResource{ .Resource = std::move(buffer), .Location = location });
        resource.BeginTrackingBuffer(m_StateTracker, initialState);
    }

    void ResourceStorage::ImportTexture(
        const ResourceId&      id,
        Rhi::Texture           texture,
        Rhi::AccessLayoutStage initialState)
    {
        CheckLockState(false);

        auto& resource          = m_Resources[id];
        auto& resourceAllocator = m_Device.get().GetResourceAllocator();

        resource.Release(m_StateTracker);
        resource.Import(TextureResource{ .Resource = std::move(texture) });
        resource.BeginTrackingTexture(m_StateTracker, initialState);
    }

    void ResourceStorage::DiscardResource(
        const ResourceId& id)
    {
        CheckLockState(false);
        auto& resource = m_Resources.at(id);
        AME_LOG_ASSERT(Log::Gfx(), resource.IsImported(), "Resource is not imported");
        m_Resources.erase(id);
    }

    //

    void ResourceStorage::UpdateResources()
    {
        CheckLockState(false);

        auto& resourceAllocator = m_Device.get().GetResourceAllocator();
        for (auto& Handle : m_Resources)
        {
            Handle.second.Reallocate(m_StateTracker, m_ResourceCache, resourceAllocator);
        }
        m_ResourceCache.ReleaseTimestamps();
    }

    BufferResource& ResourceStorage::DeclareBufferView(
        const ResourceViewId&         viewId,
        const BufferResourceViewDesc& desc)
    {
        CheckLockState(false);

        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.CreateBufferView(viewId, desc);
    }

    TextureResource& ResourceStorage::DeclareTextureView(
        const ResourceViewId&          viewId,
        const TextureResourceViewDesc& desc)
    {
        CheckLockState(false);

        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.CreateTextureView(viewId, desc);
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
} // namespace Ame::RG