#include <Rg/Resolver.hpp>
#include <Rg/ResourceStorage.hpp>
#include <Rg/Synchronizer.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rg
{
    Resolver::Resolver(
        ResourceStorage&      resourceStorage,
        ResourceSynchronizer& synchronizer) :
        m_Storage(resourceStorage),
        m_Synchronizer(synchronizer)
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
        m_Synchronizer.get().Signal(id);
    }

    void Resolver::ImportTexture(
        const ResourceId& id,
        Dg::ITexture*     texture)
    {
        m_Storage.get().ImportTexture(id, texture);
        m_Synchronizer.get().Signal(id);
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
        m_Synchronizer.get().Signal(id);

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
        m_Synchronizer.get().Signal(id);

        return texture;
    }

    //

    Co::result<Dg::IBuffer*> Resolver::GetBuffer(
        ResourceId id) const
    {
        co_await m_Synchronizer.get().WaitFor(id);
        Dg::IBuffer* buffer = nullptr;
        if (auto resource = m_Storage.get().GetResource(id))
        {
            buffer = resource->AsBuffer();
        }
        co_return buffer;
    }

    Co::result<Dg::ITexture*> Resolver::GetTexture(
        ResourceId id) const
    {
        co_await m_Synchronizer.get().WaitFor(id);
        Dg::ITexture* texture = nullptr;
        if (auto resource = m_Storage.get().GetResource(id))
        {
            texture = resource->AsTexture();
        }
        co_return texture;
    }

    Co::result<IObject*> Resolver::GetUserData(
        ResourceId id) const
    {
        co_await m_Synchronizer.get().WaitFor(id);
        IObject* userData = m_Storage.get().GetUserData(id);
        co_return userData;
    }

    //

    Co::result<void> Resolver::WriteResource(
        ResourceId id)
    {
        co_await m_Synchronizer.get().WaitFor(id);
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesWritten.emplace(id);
    }

    void Resolver::SetUserData(
        ResourceId id,
        IObject*   userData)
    {
        m_ResourcesWritten.emplace(id);
        m_Storage.get().SetUserData(id, userData);
        m_Synchronizer.get().Signal(id);
    }

    Co::result<Dg::IBuffer*> Resolver::WriteBuffer(
        ResourceId id)
    {
        co_await WriteResource(id);
        co_return m_Storage.get().GetResource(id)->AsBuffer();
    }

    Co::result<Dg::IBufferView*> Resolver::WriteBuffer(
        ResourceId                    id,
        const BufferResourceViewDesc& viewDesc)
    {
        co_await WriteResource(id);
        co_return m_Storage.get().DeclareBufferView(id, viewDesc);
    }

    Co::result<Dg::ITexture*> Resolver::WriteTexture(
        ResourceId id)
    {
        co_await WriteResource(id);
        co_return m_Storage.get().GetResource(id)->AsTexture();
    }

    Co::result<Dg::ITextureView*> Resolver::WriteTexture(
        ResourceId                     id,
        const TextureResourceViewDesc& viewDesc)
    {
        co_await WriteResource(id);
        co_return m_Storage.get().DeclareTextureView(id, viewDesc);
    }

    //

    Co::result<void> Resolver::ReadResource(
        ResourceId id)
    {
        co_await m_Synchronizer.get().WaitFor(id);
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
    }

    Co::result<IObject*> Resolver::ReadUserData(
        ResourceId id)
    {
        co_await m_Synchronizer.get().WaitFor(id);
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsUserData(id), "UserData '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
        co_return m_Storage.get().GetUserData(id);
    }

    Co::result<Dg::IBuffer*> Resolver::ReadBuffer(
        ResourceId id)
    {
        co_await ReadResource(id);
        co_return m_Storage.get().GetResource(id)->AsBuffer();
    }

    Co::result<Dg::IBufferView*> Resolver::ReadBuffer(
        ResourceId                    id,
        const BufferResourceViewDesc& viewDesc)
    {
        co_await ReadResource(id);
        co_return m_Storage.get().DeclareBufferView(id, viewDesc);
    }

    Co::result<Dg::ITexture*> Resolver::ReadTexture(
        ResourceId id)
    {
        co_await ReadResource(id);
        co_return m_Storage.get().GetResource(id)->AsTexture();
    }

    Co::result<Dg::ITextureView*> Resolver::ReadTexture(
        ResourceId                     id,
        const TextureResourceViewDesc& viewDesc)
    {
        co_await ReadResource(id);
        co_return m_Storage.get().DeclareTextureView(id, viewDesc);
    }
} // namespace Ame::Rg