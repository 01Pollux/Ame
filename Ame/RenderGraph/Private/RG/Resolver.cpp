#include <RG/Resolver.hpp>
#include <RG/ResourceStorage.hpp>
#include <Rhi/Device/Device.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    Resolver::Resolver(
        Rhi::RhiDevice&  rhiDevice,
        ResourceStorage& resourceStorage) :
        m_RhiDevice(rhiDevice),
        m_Storage(resourceStorage)
    {
    }

    //

    Rhi::RhiDevice& Resolver::GetDevice() const
    {
        return m_RhiDevice;
    }

    Dg::TEXTURE_FORMAT Resolver::GetBackbufferFormat() const
    {
        auto swapcahin = m_RhiDevice.get().GetSwapchain();
        return swapcahin->GetDesc().ColorBufferFormat;
    }

    const Dg::TextureDesc& Resolver::GetBackbufferDesc() const
    {
        auto swapcahin = m_RhiDevice.get().GetSwapchain();
        return swapcahin->GetCurrentBackBufferRTV()->GetTexture()->GetDesc();
    }

    //

    void Resolver::CreateBuffer(
        const ResourceId&     id,
        const Dg::BufferDesc& desc)
    {
        m_Storage.get().DeclareResource(id, BufferResource{ .Desc = desc });
        m_ResourcesCreated.emplace(id);
    }

    void Resolver::CreateTexture(
        const ResourceId&      id,
        const Dg::TextureDesc& desc)
    {
        m_Storage.get().DeclareResource(id, TextureResource{ .Desc = desc });
        m_ResourcesCreated.emplace(id);
    }

    //

    void Resolver::ImportBuffer(
        const ResourceId&    id,
        Dg::Ptr<Dg::IBuffer> buffer)
    {
        m_ResourcesCreated.emplace(id);
        m_Storage.get().ImportBuffer(id, std::move(buffer));
    }

    void Resolver::ImportTexture(
        const ResourceId&     id,
        Dg::Ptr<Dg::ITexture> texture)
    {
        m_ResourcesCreated.emplace(id);
        m_Storage.get().ImportTexture(id, std::move(texture));
    }

    //

    void Resolver::WriteBuffer(
        const ResourceViewId&         viewId,
        Dg::BIND_FLAGS                bindFlags,
        const BufferResourceViewDesc& viewDesc)
    {
        WriteResourceEmpty(viewId.GetResource());

        auto& buffer = m_Storage.get().DeclareBufferView(viewId, viewDesc);
        buffer.Desc.BindFlags |= bindFlags;
    }

    void Resolver::WriteTexture(
        const ResourceViewId&          viewId,
        Dg::BIND_FLAGS                 bindFlags,
        const TextureResourceViewDesc& viewDesc)
    {
        WriteResourceEmpty(viewId.GetResource());

        auto& texture = m_Storage.get().DeclareTextureView(viewId, viewDesc);
        texture.Desc.BindFlags |= bindFlags;
    }

    void Resolver::WriteResourceEmpty(
        const ResourceId& id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesWritten.emplace(id);
    }

    //

    void Resolver::ReadBuffer(
        const ResourceViewId&         viewId,
        Dg::BIND_FLAGS                bindFlags,
        const BufferResourceViewDesc& viewDesc)
    {
        ReadResourceEmpty(viewId.GetResource());
        auto& buffer = m_Storage.get().DeclareBufferView(viewId, viewDesc);
        buffer.Desc.BindFlags |= bindFlags;
    }

    void Resolver::ReadTexture(
        const ResourceViewId&          viewId,
        Dg::BIND_FLAGS                 bindFlags,
        const TextureResourceViewDesc& viewDesc)
    {
        ReadResourceEmpty(viewId.GetResource());

        auto& texture = m_Storage.get().DeclareTextureView(viewId, viewDesc);
        texture.Desc.BindFlags |= bindFlags;
    }

    void Resolver::ReadResourceEmpty(
        const ResourceId& id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.get().ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
    }
} // namespace Ame::RG