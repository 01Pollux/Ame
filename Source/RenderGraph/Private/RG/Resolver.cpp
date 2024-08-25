#include <Rg/Resolver.hpp>
#include <Rg/ResourceStorage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rg
{
    Resolver::Resolver(
        ResourceStorage& resourceStorage) :
        m_Storage(resourceStorage)
    {
    }

    //

    Rhi::IRhiDevice* Resolver::GetDevice() const noexcept
    {
        return m_Storage.get().GetDevice();
    }

    Dg::TEXTURE_FORMAT Resolver::GetBackbufferFormat() const
    {
        return m_Storage.get().GetBackbufferFormat();
    }

    const Dg::TextureDesc& Resolver::GetBackbufferDesc() const
    {
        return m_Storage.get().GetBackbufferDesc();
    }

    //

    void Resolver::ImportBuffer(
        const ResourceId& id,
        Dg::IBuffer*      buffer)
    {
        m_Storage.get().ImportBuffer(id, buffer);
    }

    void Resolver::ImportTexture(
        const ResourceId& id,
        Dg::ITexture*     texture)
    {
        m_Storage.get().ImportTexture(id, texture);
    }

    void Resolver::CreateBuffer(
        const ResourceId&     id,
        Rhi::BufferInitData*  initData,
        const Dg::BufferDesc& desc)
    {
        m_Storage.get().DeclareResource(id, initData, desc);
    }

    void Resolver::CreateTexture(
        const ResourceId&      id,
        Rhi::TextureInitData*  initData,
        const Dg::TextureDesc& desc)
    {
        m_Storage.get().DeclareResource(id, initData, desc);
    }

    //

    IObject* Resolver::GetUserData(
        const ResourceId& id) const
    {
        IObject* userData = m_Storage.get().GetUserData(id);
        return userData;
    }

    //

    void Resolver::WriteResource(
        const ResourceId& id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesWritten.emplace(id);
    }

    void Resolver::SetUserData(
        const ResourceId& id,
        IObject*          userData)
    {
        m_ResourcesWritten.emplace(id);
        m_Storage.get().SetUserData(id, userData);
    }

    ResourceViewId Resolver::WriteBuffer(
        const ResourceId&             id,
        Dg::BIND_FLAGS                bindFlags,
        const BufferResourceViewDesc& viewDesc)
    {
        WriteResource(id);
        return DeclareView(id, bindFlags, viewDesc);
    }

    ResourceViewId Resolver::WriteTexture(
        const ResourceId&              id,
        Dg::BIND_FLAGS                 bindFlags,
        const TextureResourceViewDesc& viewDesc)
    {
        WriteResource(id);
        return DeclareView(id, bindFlags, viewDesc);
    }

    //

    void Resolver::ReadResource(
        const ResourceId& id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
    }

    void Resolver::ReadUserData(
        const ResourceId& id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsUserData(id), "UserData '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
    }

    ResourceViewId Resolver::ReadBuffer(
        const ResourceId&             id,
        Dg::BIND_FLAGS                bindFlags,
        const BufferResourceViewDesc& viewDesc)
    {
        ReadResource(id);
        return DeclareView(id, bindFlags, viewDesc);
    }

    ResourceViewId Resolver::ReadTexture(
        const ResourceId&              id,
        Dg::BIND_FLAGS                 bindFlags,
        const TextureResourceViewDesc& viewDesc)
    {
        ReadResource(id);
        return DeclareView(id, bindFlags, viewDesc);
    }

    //

    ResourceViewId Resolver::DeclareView(
        const ResourceId&             id,
        Dg::BIND_FLAGS                bindFlags,
        const BufferResourceViewDesc& viewDesc)
    {
        auto viewId = m_Storage.get().DeclareBufferView(id, viewDesc);
        auto handle = m_Storage.get().GetResourceMut(id);
        if (!handle->IsImported())
        {
            auto& bufferDesc = m_Storage.get().GetResourceMut(id)->GetBufferDesc();
            bufferDesc.BindFlags |= bindFlags;
        }
        return viewId;
    }

    ResourceViewId Resolver::DeclareView(
        const ResourceId&              id,
        Dg::BIND_FLAGS                 bindFlags,
        const TextureResourceViewDesc& viewDesc)
    {
        auto viewId = m_Storage.get().DeclareTextureView(id, viewDesc);
        auto handle = m_Storage.get().GetResourceMut(id);
        if (!handle->IsImported())
        {
            auto& textureDesc = m_Storage.get().GetResourceMut(id)->GetTextureDesc();
            textureDesc.BindFlags |= bindFlags;
        }
        return viewId;
    }
} // namespace Ame::Rg