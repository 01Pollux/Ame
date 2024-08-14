#include <RG/Resolver.hpp>
#include <RG/ResourceStorage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
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

    const Dg::BufferDesc* Resolver::GetBufferDesc(
        const ResourceId& id) const
    {
        if (auto buffer = m_Storage.get().GetResource(id)->AsBuffer())
        {
            return &buffer->Desc;
        }
        return nullptr;
    }

    const Dg::TextureDesc* Resolver::GetTextureDesc(
        const ResourceId& id) const
    {
        if (auto texture = m_Storage.get().GetResource(id)->AsTexture())
        {
            return &texture->Desc;
        }
        return nullptr;
    }

    //

    void Resolver::ImportBuffer(
        const ResourceId& id,
        Ptr<Dg::IBuffer>  buffer)
    {
        m_ResourcesCreated.emplace(id);
        m_Storage.get().ImportBuffer(id, std::move(buffer));
    }

    void Resolver::ImportTexture(
        const ResourceId& id,
        Ptr<Dg::ITexture> texture)
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

        if (bindFlags & Dg::BIND_FLAGS::BIND_RENDER_TARGET)
        {
#ifdef AME_DEBUG
            AME_LOG_ASSERT(Log::Gfx(), m_RenderTargets.size() <= 8, "Too many render targets");
            AME_LOG_ASSERT(Log::Gfx(),
                           std::holds_alternative<RenderTargetViewDesc>(viewDesc) ||
                               (std::holds_alternative<Dg::TEXTURE_VIEW_TYPE>(viewDesc) && std::get<Dg::TEXTURE_VIEW_TYPE>(viewDesc) == Dg::TEXTURE_VIEW_RENDER_TARGET),
                           "View '{}' is not of a render target", viewId.GetResource().GetName());
#endif
            m_RenderTargets.emplace_back(viewId);
        }
        else if (bindFlags & Dg::BIND_FLAGS::BIND_DEPTH_STENCIL)
        {
#ifdef AME_DEBUG
            AME_LOG_ASSERT(Log::Gfx(), !m_DepthStencil, "Depth stencil was already set");

            AME_LOG_ASSERT(Log::Gfx(),
                           std::holds_alternative<DepthStencilViewDesc>(viewDesc) ||
                               (std::holds_alternative<Dg::TEXTURE_VIEW_TYPE>(viewDesc) && std::get<Dg::TEXTURE_VIEW_TYPE>(viewDesc) == Dg::TEXTURE_VIEW_DEPTH_STENCIL),
                           "View '{}' is not of a depth stencil", viewId.GetResource().GetName());
#endif
            m_DepthStencil = viewId;
        }
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

    //

    void Resolver::WriteUserData(
        const ResourceId& id,
        IObject*          userData)
    {
        m_ResourcesWritten.emplace(id);
        m_Storage.get().SetUserData(id, userData);
    }

    void Resolver::ReadUserData(
        const ResourceId& id)
    {
        m_ResourcesRead.emplace(id);
    }
} // namespace Ame::RG