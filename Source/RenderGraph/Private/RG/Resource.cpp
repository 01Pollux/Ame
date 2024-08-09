#include <RG/Resource.hpp>

#include <DiligentCore/Common/interface/HashUtils.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    ResourceHandle::operator bool() const noexcept
    {
        return !std::holds_alternative<std::monostate>(m_Resource);
    }

    //

    void ResourceHandle::Import(
        const ResourceId& id,
        RhiResource&&     resource)
    {
        m_Resource = std::move(resource);
        InitializeViewMap();

        std::visit(
            VariantVisitor{
                [&](std::monostate) {},
                [&](auto& data)
                {
                    if (data.Resource)
                    {
                        data.Desc = data.Resource->GetDesc();
                    }
                } },
            m_Resource);

        m_IsImported = true;

#ifndef AME_DIST
        m_Name = id.GetName();
#endif
    }

    void ResourceHandle::SetDynamic(
        const ResourceId& id,
        RhiResource&&     resource)
    {
        m_Resource = std::move(resource);
        InitializeViewMap();

        m_IsImported = false;

#ifndef AME_DIST
        m_Name = id.GetName();
#endif
    }

    //

    const RhiResource& ResourceHandle::Get() const noexcept
    {
        return m_Resource;
    }

    const TextureResource* ResourceHandle::AsTexture() const noexcept
    {
        return std::get_if<TextureResource>(&m_Resource);
    }

    TextureResource* ResourceHandle::AsTexture() noexcept
    {
        return std::get_if<TextureResource>(&m_Resource);
    }

    const BufferResource* ResourceHandle::AsBuffer() const noexcept
    {
        return std::get_if<BufferResource>(&m_Resource);
    }

    BufferResource* ResourceHandle::AsBuffer() noexcept
    {
        return std::get_if<BufferResource>(&m_Resource);
    }

    //

    BufferResource& ResourceHandle::CreateBufferView(
        const ResourceViewId&         viewId,
        const BufferResourceViewDesc& desc)
    {
        auto viewMap = std::get_if<RhiBufferViewMap>(&m_Views);
        AME_LOG_ASSERT(Log::Gfx(), viewMap != nullptr, "Resource is not a buffer");

        (*viewMap)[viewId].Desc = desc;
        return std::get<BufferResource>(m_Resource);
    }

    TextureResource& ResourceHandle::CreateTextureView(
        const ResourceViewId&          viewId,
        const TextureResourceViewDesc& desc)
    {
        auto viewMap = std::get_if<RhiTextureViewMap>(&m_Views);
        AME_LOG_ASSERT(Log::Gfx(), viewMap != nullptr, "Resource is not a texture");

        (*viewMap)[viewId].Desc = desc;
        return std::get<TextureResource>(m_Resource);
    }

    //

    const BufferResourceView* ResourceHandle::GetBufferView(
        const ResourceViewId& viewId) const noexcept
    {
        const BufferResourceView* view = nullptr;
        if (auto viewMap = std::get_if<RhiBufferViewMap>(&m_Views))
        {
            auto iter = viewMap->find(viewId);
            view      = iter != viewMap->end() ? &iter->second : nullptr;
        }
        return view;
    }

    const TextureResourceView* ResourceHandle::GetTextureView(
        const ResourceViewId& viewId) const noexcept
    {
        const TextureResourceView* view = nullptr;
        if (auto viewMap = std::get_if<RhiTextureViewMap>(&m_Views))
        {
            auto iter = viewMap->find(viewId);
            view      = iter != viewMap->end() ? &iter->second : nullptr;
        }
        return view;
    }

    RhiResourceView ResourceHandle::GetView(
        const ResourceViewId& viewId) const noexcept
    {
        RhiResourceView view = std::monostate{};
        std::visit(
            VariantVisitor{
                [&](std::monostate) {},
                [&](const auto& viewMap)
                {
                    auto iter = viewMap.find(viewId);
                    if (iter != viewMap.end())
                    {
                        view = iter->second;
                    }
                } },
            m_Views);
        return view;
    }

    //

    bool ResourceHandle::IsImported() const noexcept
    {
        return m_IsImported;
    }

    //

    void ResourceHandle::Reallocate(
        Dg::IRenderDevice* renderDevice)
    {
        std::visit(
            VariantVisitor{
                [](std::monostate) {},
                [&](auto& data)
                {
                    if (!IsImported())
                    {
                        using descType = std::decay_t<decltype(data.Desc)>;
                        if (!data.Resource || data.Resource->GetDesc() != data.Desc)
                        {
#ifndef AME_DIST
                            data.Desc.Name = m_Name.c_str();
#endif
                            data.Resource.Release();
                            if constexpr (std::is_same_v<descType, Dg::TextureDesc>)
                            {
                                renderDevice->CreateTexture(data.Desc, nullptr, &data.Resource);
                            }
                            else
                            {
                                renderDevice->CreateBuffer(data.Desc, nullptr, &data.Resource);
                            }
                        }
                    }

                    RecreateViewsIfNeeded(data);
                } },
            m_Resource);
    }

    //

    void ResourceHandle::InitializeViewMap()
    {
        if (m_Views.index() != m_Resource.index())
        {
            std::visit(
                VariantVisitor{
                    [&](std::monostate)
                    {
                        m_Views = {};
                    },
                    [&](const BufferResource&)
                    {
                        m_Views = RhiBufferViewMap{};
                    },
                    [&](const TextureResource&)
                    {
                        m_Views = RhiTextureViewMap{};
                    } },
                m_Resource);
        }
    }

    //

    void ResourceHandle::RecreateViewsIfNeeded(
        BufferResource& bufferResource)
    {
        auto computeViewHash = [](const Dg::BufferViewDesc& desc)
        {
            return Dg::ComputeHash(
                desc.ViewType,
                desc.Format.ValueType, desc.Format.NumComponents, desc.Format.IsNormalized,
                desc.ByteOffset,
                desc.ByteWidth);
        };

        auto& viewMap = std::get<RhiBufferViewMap>(m_Views);
        for (auto& [id, view] : viewMap)
        {
            std::visit(
                VariantVisitor{
                    [&](Dg::BUFFER_VIEW_TYPE type)
                    {
                        if (view.Hash != type)
                        {
                            view.View.Release();
                            view.View = bufferResource.Resource->GetDefaultView(type);
                            view.Hash = type;
                        }
                    },
                    [&](const Dg::BufferViewDesc& desc)
                    {
                        size_t hash = computeViewHash(desc);
                        if (view.Hash != hash)
                        {
                            view.View.Release();
                            bufferResource.Resource->CreateView(desc, &view.View);
                            view.Hash = hash;
                        }
                    } },
                view.Desc);
        }
    }

    void ResourceHandle::RecreateViewsIfNeeded(
        TextureResource& textureResource)
    {
        auto computeViewHash = [](const auto& desc)
        {
            using viewType = std::decay_t<decltype(desc)>;
            size_t hash    = Dg::ComputeHash(static_cast<const Dg::TextureViewDesc&>(desc));

            if constexpr (std::is_same_v<viewType, RenderTargetViewDesc>)
            {
                Dg::HashCombine(
                    hash,
                    desc.ClearColor.r(), desc.ClearColor.g(), desc.ClearColor.b(), desc.ClearColor.a(),
                    std::to_underlying(desc.ClearType),
                    desc.ForceColor);
            }
            else if constexpr (std::is_same_v<viewType, DepthStencilViewDesc>)
            {
                Dg::HashCombine(
                    hash,
                    desc.Depth, desc.Stencil,
                    std::to_underlying(desc.ClearType),
                    desc.ForceDepth, desc.ForceStencil);
            }

            return hash;
        };

        auto& viewMap = std::get<RhiTextureViewMap>(m_Views);
        for (auto& [id, view] : viewMap)
        {
            std::visit(
                VariantVisitor{
                    [&](Dg::TEXTURE_VIEW_TYPE type)
                    {
                        if (view.Hash != type)
                        {
                            view.View.Release();
                            view.View = textureResource.Resource->GetDefaultView(type);
                            view.Hash = type;
                        }
                    },
                    [&](auto& desc)
                    {
                        size_t hash = computeViewHash(desc);
                        if (view.Hash != hash)
                        {
                            view.View.Release();
                            if (desc.ViewType == Dg::TEXTURE_VIEW_UNDEFINED)
                            {
                                if constexpr (std::is_same_v<std::decay_t<decltype(desc)>, RenderTargetViewDesc>)
                                {
                                    desc.ViewType = Dg::TEXTURE_VIEW_RENDER_TARGET;
                                }
                                else if constexpr (std::is_same_v<std::decay_t<decltype(desc)>, DepthStencilViewDesc>)
                                {
                                    desc.ViewType = Dg::TEXTURE_VIEW_DEPTH_STENCIL;
                                }
                            }
                            textureResource.Resource->CreateView(desc, &view.View);
                            view.Hash = hash;
                        }
                    } },
                view.Desc);
        }
    }
} // namespace Ame::RG