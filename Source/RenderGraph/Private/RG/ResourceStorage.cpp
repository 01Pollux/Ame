#include <RG/ResourceStorage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
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
        return std::holds_alternative<std::monostate>(iter->second.GetView(viewId));
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

    RhiResourceView ResourceStorage::GetResourceView(
        const ResourceViewId& viewId) const
    {
        auto iter = m_Resources.find(viewId.GetResource());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.GetView(viewId);
    }

    Ptr<Dg::IObject> ResourceStorage::GetResourceViewHandle(
        const ResourceViewId& viewId) const
    {
        auto view = GetResourceView(viewId);
        return std::visit(
            VariantVisitor{
                [](std::monostate) -> Ptr<Dg::IObject>
                { return {}; },
                [&](const auto& resource) -> Ptr<Dg::IObject>
                { return resource.get().View; } },
            view);
    }

    //

    void ResourceStorage::DeclareResource(
        const ResourceId& id,
        RhiResource       resource)
    {
        CheckLockState(false);
        AME_LOG_ASSERT(Log::Gfx(), !m_Resources.contains(id), "Resource already exists");
        m_Resources[id].SetDynamic(id, std::move(resource));
    }

    //

    void ResourceStorage::ImportBuffer(
        const ResourceId& id,
        Ptr<Dg::IBuffer>  buffer)
    {
        CheckLockState(false);

        auto& resource = m_Resources[id];
        resource.Import(BufferResource{ .Resource = std::move(buffer) });
    }

    void ResourceStorage::ImportTexture(
        const ResourceId& id,
        Ptr<Dg::ITexture> texture)
    {
        CheckLockState(false);

        auto& resource = m_Resources[id];
        resource.Import(TextureResource{ .Resource = std::move(texture) });
    }

    void ResourceStorage::DiscardResource(
        const ResourceId& id)
    {
        CheckLockState(false);

        auto& resource = m_Resources.at(id);
        AME_LOG_ASSERT(Log::Gfx(), resource.IsImported(), "Resource is not imported");
        m_Resources.erase(id);

        SetRebuildState(true);
    }

    //

    void ResourceStorage::UpdateResources(
        Dg::IRenderDevice* renderDevice)
    {
        CheckLockState(false);
        for (auto& handle : m_Resources | std::views::values)
        {
            handle.Reallocate(renderDevice);
        }
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

    //

    bool ResourceStorage::NeedsRebuild() const noexcept
    {
        return m_NeedRebuild;
    }

    void ResourceStorage::SetRebuildState(
        bool state) noexcept
    {
        m_NeedRebuild = state;
    }
} // namespace Ame::RG