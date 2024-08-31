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

    bool ResourceStorage::ContainsResource(const ResourceId& id) const
    {
        return m_Resources.contains(id);
    }

    const ResourceHandle* ResourceStorage::GetResource(const ResourceId& id) const
    {
        auto iter = m_Resources.find(id);
        return iter != m_Resources.end() ? &iter->second : nullptr;
    }

    ResourceHandle* ResourceStorage::GetResourceMut(const ResourceId& id)
    {
        CheckLockState(false);
        auto iter = m_Resources.find(id);
        return iter != m_Resources.end() ? &iter->second : nullptr;
    }

    //

    Dg::IBufferView* ResourceStorage::GetBufferView(ResourceViewId viewId) const
    {
        auto iter = m_Resources.find(viewId.GetResourceId());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource of id '{}' doesn't exists",
                       viewId.GetResourceId().GetId());
        return iter->second.GetBufferView(viewId.GetId());
    }

    Dg::ITextureView* ResourceStorage::GetTextureView(ResourceViewId viewId) const
    {
        auto iter = m_Resources.find(viewId.GetResourceId());
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource of id '{}' doesn't exists",
                       viewId.GetResourceId().GetId());
        return iter->second.GetTextureView(viewId.GetId());
    }

    //

    bool ResourceStorage::ContainsUserData(const ResourceId& id) const
    {
        return m_UserDatas.contains(id);
    }

    void ResourceStorage::SetUserData(const ResourceId& id, IObject* userData)
    {
        CheckLockState(false);
        m_UserDatas[id] = userData;
    }

    IObject* ResourceStorage::GetUserData(const ResourceId& id) const
    {
        auto iter = m_UserDatas.find(id);
        return iter != m_UserDatas.end() ? iter->second : nullptr;
    }

    //

    void ResourceStorage::DeclareResource(const ResourceId& id, Rhi::BufferInitData* initData,
                                          const Dg::BufferDesc& desc)
    {
        CheckLockState(false);
        AME_LOG_ASSERT(Log::Gfx(), !m_Resources.contains(id), "Resource '{}' already exists", id.GetName());
        m_Resources[id].SetLocal(initData, desc);
    }

    void ResourceStorage::DeclareResource(const ResourceId& id, Rhi::TextureInitData* initData,
                                          const Dg::TextureDesc& desc)
    {
        CheckLockState(false);
        AME_LOG_ASSERT(Log::Gfx(), !m_Resources.contains(id), "Resource '{}' already exists", id.GetName());
        m_Resources[id].SetLocal(initData, desc);
    }

    //

    void ResourceStorage::ImportBuffer(const ResourceId& id, Dg::IBuffer* buffer)
    {
        CheckLockState(false);

        m_Resources[id].Import(buffer);
    }

    void ResourceStorage::ImportTexture(const ResourceId& id, Dg::ITexture* texture)
    {
        CheckLockState(false);
        m_Resources[id].Import(texture);
    }

    void ResourceStorage::DiscardResource(const ResourceId& id)
    {
        CheckLockState(false);

        auto& resource = m_Resources.at(id);
        AME_LOG_ASSERT(Log::Gfx(), resource.IsImported(), "Resource '{}' is not imported", id.GetName());
        m_Resources.erase(id);

        SetRebuildState(true);
    }

    //

    void ResourceStorage::UpdateResources()
    {
        auto renderDevice = m_RhiDevice->GetRenderDevice();
        CheckLockState(false);
        for (auto& pair : m_Resources)
        {
            pair.second.Reallocate(renderDevice);
        }
    }

    void ResourceStorage::ClearResources()
    {
        CheckLockState(false);
        std::erase_if(m_Resources, [](const auto& pair) { return !pair.second.IsImported(); });
    }

    ResourceViewId ResourceStorage::DeclareBufferView(const ResourceId& id, const BufferResourceViewDesc& desc)
    {
        CheckLockState(false);

        auto iter = m_Resources.find(id);
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource '{}' doesn't exists", id.GetName());
        return ResourceViewId(id, iter->second.CreateView(desc));
    }

    ResourceViewId ResourceStorage::DeclareTextureView(const ResourceId& id, const TextureResourceViewDesc& desc)
    {
        CheckLockState(false);

        auto iter = m_Resources.find(id);
        AME_LOG_ASSERT(Log::Gfx(), iter != m_Resources.end(), "Resource '{}' doesn't exists", id.GetName());
        return ResourceViewId(id, iter->second.CreateView(desc));
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

    void ResourceStorage::CheckLockState(bool locked) const
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

    void ResourceStorage::SetRebuildState(bool state) noexcept
    {
        m_NeedRebuild = state;
    }
} // namespace Ame::Rg