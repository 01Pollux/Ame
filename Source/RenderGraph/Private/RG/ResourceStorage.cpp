#include <Rg/ResourceStorage.hpp>
#include <Rhi/Device/RhiDevice.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rg
{
    Rhi::IRhiDevice* ResourceStorage::GetDevice() const noexcept
    {
        return m_RhiDevice;
    }

    Dg::TEXTURE_FORMAT ResourceStorage::GetBackbufferFormat() const
    {
        auto swapchain = GetDevice()->GetSwapchain();
        return swapchain->GetDesc().ColorBufferFormat;
    }

    const Dg::TextureDesc& ResourceStorage::GetBackbufferDesc() const
    {
        auto swapchain = GetDevice()->GetSwapchain();
        return swapchain->GetCurrentBackBufferRTV()->GetTexture()->GetDesc();
    }

    //

    bool ResourceStorage::ContainsResource(
        const ResourceId& id) const
    {
        ResourceId::Validate(id);
        return m_Resources.contains(id);
    }

    const ResourceHandle* ResourceStorage::GetResource(
        const ResourceId& id) const
    {
        ResourceId::Validate(id);
        auto iter = m_Resources.find(id);
        return iter != m_Resources.end() ? &iter->second : nullptr;
    }

    ResourceHandle* ResourceStorage::GetResourceMut(
        const ResourceId& id)
    {
        ResourceId::Validate(id);
        CheckLockState(false);
        auto iter = m_Resources.find(id);
        return iter != m_Resources.end() ? &iter->second : nullptr;
    }

    //

    bool ResourceStorage::ContainsUserData(
        const ResourceId& id) const
    {
        ResourceId::Validate(id);
        return m_UserDatas.contains(id);
    }

    void ResourceStorage::SetUserData(
        const ResourceId& id,
        IObject*          userData)
    {
        ResourceId::Validate(id);
        CheckLockState(false);
        m_UserDatas[id] = userData;
    }

    IObject* ResourceStorage::GetUserData(
        const ResourceId& id) const
    {
        ResourceId::Validate(id);
        auto iter = m_UserDatas.find(id);
        return iter != m_UserDatas.end() ? iter->second : nullptr;
    }

    //

    void ResourceStorage::DeclareResource(
        const ResourceId& id,
        RhiResource       resource)
    {
        ResourceId::Validate(id);
        CheckLockState(false);
        AME_LOG_ASSERT(Log::Gfx(), !m_Resources.contains(id), "Resource already exists");
        m_Resources[id].SetLocal(id, std::move(resource));
    }

    //

    void ResourceStorage::ImportBuffer(
        const ResourceId& id,
        Dg::IBuffer*      buffer)
    {
        ResourceId::Validate(id);
        CheckLockState(false);

        auto& resource = m_Resources[id];
        resource.Import(id, Ptr{ buffer });
    }

    void ResourceStorage::ImportTexture(
        const ResourceId& id,
        Dg::ITexture*     texture)
    {
        ResourceId::Validate(id);
        CheckLockState(false);

        auto& resource = m_Resources[id];
        resource.Import(id, Ptr{ texture });
    }

    void ResourceStorage::DiscardResource(
        const ResourceId& id)
    {
        ResourceId::Validate(id);
        CheckLockState(false);

        auto& resource = m_Resources.at(id);
        AME_LOG_ASSERT(Log::Gfx(), resource.IsImported(), "Resource is not imported");
        m_Resources.erase(id);

        SetRebuildState(true);
    }

    //

    void ResourceStorage::ClearResources()
    {
        CheckLockState(false);
        std::erase_if(m_Resources, [](const auto& pair)
                      { return !pair.second.IsImported(); });
    }

    Dg::IBufferView* ResourceStorage::DeclareBufferView(
        const ResourceId&             id,
        const BufferResourceViewDesc& desc)
    {
        ResourceId::Validate(id);
        CheckLockState(false);

        auto iter = m_Resources.find(id);
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.LoadView(desc);
    }

    Dg::ITextureView* ResourceStorage::DeclareTextureView(
        const ResourceId&              id,
        const TextureResourceViewDesc& desc)
    {
        ResourceId::Validate(id);
        CheckLockState(false);

        auto iter = m_Resources.find(id);
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource doesn't exists");
        return iter->second.LoadView(desc);
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
} // namespace Ame::Rg