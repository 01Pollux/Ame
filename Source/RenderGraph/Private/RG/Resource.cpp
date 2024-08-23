#include <Rg/Resource.hpp>

#include <DiligentCore/Common/interface/HashUtils.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rg
{
    void ResourceId::Validate(
        const ResourceId& id)
    {
#ifndef AME_DIST
        AME_LOG_ASSERT(Log::Gfx(), static_cast<bool>(id) && id.GetName(), "Invalid resource id");
#endif
    }

    //

    void ResourceHandle::Import(
        const ResourceId& id,
        RhiResource       resource)
    {
        m_Resource   = std::move(resource);
        m_IsImported = true;

        InitializeViewMap();

#ifndef AME_DIST
        m_Name = id.GetName();
#endif
    }

    void ResourceHandle::SetLocal(
        const ResourceId& id,
        RhiResource       resource)
    {
        m_Resource   = std::move(resource);
        m_IsImported = false;

        InitializeViewMap();

#ifndef AME_DIST
        m_Name = id.GetName();
#endif
    }

    //

    const RhiResource& ResourceHandle::Get() const noexcept
    {
        return m_Resource;
    }

    Dg::ITexture* ResourceHandle::AsTexture() const noexcept
    {
        Dg::ITexture* texture = nullptr;
        if (auto resource = std::get_if<Ptr<Dg::ITexture>>(&m_Resource))
        {
            texture = *resource;
        }
        return texture;
    }

    Dg::IBuffer* ResourceHandle::AsBuffer() const noexcept
    {
        Dg::IBuffer* buffer = nullptr;
        if (auto resource = std::get_if<Ptr<Dg::IBuffer>>(&m_Resource))
        {
            buffer = *resource;
        }
        return buffer;
    }

    //

    [[nodiscard]] static size_t ComputeViewHash(
        const BufferResourceViewDesc& viewDesc)
    {
        return std::visit(
            VariantVisitor{
                [](Dg::BUFFER_VIEW_TYPE type)
                {
                    return static_cast<size_t>(type);
                },
                [](const Dg::BufferViewDesc& desc)
                {
                    return Dg::ComputeHash(
                        desc.ViewType,
                        desc.Format.ValueType, desc.Format.NumComponents, desc.Format.IsNormalized,
                        desc.ByteOffset,
                        desc.ByteWidth);
                } },
            viewDesc);
    }

    [[nodiscard]] static size_t ComputeViewHash(
        const TextureResourceViewDesc& viewDesc)
    {
        return std::visit(
            VariantVisitor{
                [](Dg::TEXTURE_VIEW_TYPE type)
                {
                    return static_cast<size_t>(type);
                },
                [](const auto& desc)
                {
                    using viewType = std::decay_t<decltype(desc)>;
                    return Dg::ComputeHash(static_cast<const Dg::TextureViewDesc&>(desc));
                } },
            viewDesc);
    }

    Dg::IBufferView* ResourceHandle::LoadView(
        const BufferResourceViewDesc& viewDesc)
    {
        size_t viewHash = ComputeViewHash(viewDesc);

        auto& viewMap = std::get<RhiBufferViewMap>(m_Views);
        auto& buffer  = std::get<Ptr<Dg::IBuffer>>(m_Resource);

        auto& view = viewMap[viewHash];
        if (!view)
        {
            std::visit(
                VariantVisitor{
                    [&](Dg::BUFFER_VIEW_TYPE type)
                    {
                        view = buffer->GetDefaultView(type);
                    },
                    [&](const Dg::BufferViewDesc& desc)
                    {
                        buffer->CreateView(desc, &view);
                    } },
                viewDesc);
        }
        return view;
    }

    Dg::ITextureView* ResourceHandle::LoadView(
        const TextureResourceViewDesc& viewDesc)
    {
        size_t viewHash = ComputeViewHash(viewDesc);

        auto& viewMap = std::get<RhiTextureViewMap>(m_Views);
        auto& texture = std::get<Ptr<Dg::ITexture>>(m_Resource);

        auto& view = viewMap[viewHash];
        if (!view)
        {
            std::visit(
                VariantVisitor{
                    [&](Dg::TEXTURE_VIEW_TYPE type)
                    {
                        view = texture->GetDefaultView(type);
                    },
                    [&](const auto& desc)
                    {
                        Dg::TextureViewDesc viewDesc = desc;
                        viewDesc.Name                = m_Name.c_str();
                        texture->CreateView(viewDesc, &view);
                    } },
                viewDesc);
        }
        return view;
    }

    //

    bool ResourceHandle::IsImported() const noexcept
    {
        return m_IsImported;
    }

    //

    void ResourceHandle::InitializeViewMap()
    {
        if (m_Views.index() != m_Resource.index())
        {
            std::visit(
                VariantVisitor{
                    [&](const Ptr<Dg::IBuffer>&)
                    {
                        m_Views = RhiBufferViewMap{};
                    },
                    [&](const Ptr<Dg::ITexture>&)
                    {
                        m_Views = RhiTextureViewMap{};
                    } },
                m_Resource);
        }
    }
} // namespace Ame::Rg