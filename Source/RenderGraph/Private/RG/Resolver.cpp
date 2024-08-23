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

    Dg::IBuffer* Resolver::CreateBuffer(
        const ResourceId&     id,
        const Dg::BufferData* initData,
        const Dg::BufferDesc& desc)
    {
        auto rhiDevice    = m_Storage.get().GetDevice();
        auto renderDevice = rhiDevice->GetRenderDevice();

        Ptr<Dg::IBuffer> buffer;
        renderDevice->CreateBuffer(desc, initData, &buffer);
        m_Storage.get().DeclareResource(id, buffer);

        return buffer;
    }

    Dg::ITexture* Resolver::CreateTexture(
        const ResourceId&      id,
        const Dg::TextureData* initData,
        const Dg::TextureDesc& desc)
    {
        auto rhiDevice    = m_Storage.get().GetDevice();
        auto renderDevice = rhiDevice->GetRenderDevice();

        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(desc, initData, &texture);
        m_Storage.get().DeclareResource(id, texture);

        return texture;
    }

    //

    Dg::IBuffer* Resolver::GetBuffer(
        ResourceId id) const
    {
        Dg::IBuffer* buffer = nullptr;
        if (auto resource = m_Storage.get().GetResource(id))
        {
            buffer = resource->AsBuffer();
        }
        return buffer;
    }

    Dg::ITexture* Resolver::GetTexture(
        ResourceId id) const
    {
        Dg::ITexture* texture = nullptr;
        if (auto resource = m_Storage.get().GetResource(id))
        {
            texture = resource->AsTexture();
        }
        return texture;
    }

    IObject* Resolver::GetUserData(
        ResourceId id) const
    {
        IObject* userData = m_Storage.get().GetUserData(id);
        return userData;
    }

    //

    void Resolver::WriteResource(
        ResourceId id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesWritten.emplace(id);
    }

    void Resolver::SetUserData(
        ResourceId id,
        IObject*   userData)
    {
        m_ResourcesWritten.emplace(id);
        m_Storage.get().SetUserData(id, userData);
    }

    Dg::IBuffer* Resolver::WriteBuffer(
        ResourceId id)
    {
        WriteResource(id);
        return m_Storage.get().GetResource(id)->AsBuffer();
    }

    Dg::IBufferView* Resolver::WriteBuffer(
        ResourceId                    id,
        const BufferResourceViewDesc& viewDesc)
    {
        WriteResource(id);
        return m_Storage.get().DeclareBufferView(id, viewDesc);
    }

    Dg::ITexture* Resolver::WriteTexture(
        ResourceId id)
    {
        WriteResource(id);
        return m_Storage.get().GetResource(id)->AsTexture();
    }

    Dg::ITextureView* Resolver::WriteTexture(
        ResourceId                     id,
        const TextureResourceViewDesc& viewDesc)
    {
        WriteResource(id);
        return m_Storage.get().DeclareTextureView(id, viewDesc);
    }

    //

    void Resolver::ReadResource(
        ResourceId id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
    }

    IObject* Resolver::ReadUserData(
        ResourceId id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsUserData(id), "UserData '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
        return m_Storage.get().GetUserData(id);
    }

    Dg::IBuffer* Resolver::ReadBuffer(
        ResourceId id)
    {
        ReadResource(id);
        return m_Storage.get().GetResource(id)->AsBuffer();
    }

    Dg::IBufferView* Resolver::ReadBuffer(
        ResourceId                    id,
        const BufferResourceViewDesc& viewDesc)
    {
        ReadResource(id);
        return m_Storage.get().DeclareBufferView(id, viewDesc);
    }

    Dg::ITexture* Resolver::ReadTexture(
        ResourceId id)
    {
        ReadResource(id);
        return m_Storage.get().GetResource(id)->AsTexture();
    }

    Dg::ITextureView* Resolver::ReadTexture(
        ResourceId                     id,
        const TextureResourceViewDesc& viewDesc)
    {
        ReadResource(id);
        return m_Storage.get().DeclareTextureView(id, viewDesc);
    }
} // namespace Ame::Rg