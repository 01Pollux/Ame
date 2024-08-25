#include <Rg/Resource.hpp>
#include <DiligentCore/Common/interface/HashUtils.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rg
{
    bool ResourceHandle::IsImported() const noexcept
    {
        return m_IsImported;
    }

    //

    void ResourceHandle::Import(
        Dg::IBuffer* resource)
    {
        m_Handle     = BufferHandle{ { Ptr{ resource }, {}, resource->GetDesc() } };
        m_IsImported = false;
    }

    void ResourceHandle::Import(
        Dg::ITexture* resource)
    {
        m_Handle     = TextureHandle{ Ptr{ resource }, {}, resource->GetDesc() };
        m_IsImported = false;
    }

    void ResourceHandle::SetLocal(
        Rhi::BufferInitData*  initData,
        const Dg::BufferDesc& desc)
    {
        m_Handle     = BufferHandle{ { {}, Ptr{ initData }, desc } };
        m_IsImported = false;
    }

    void ResourceHandle::SetLocal(
        Rhi::TextureInitData*  initData,
        const Dg::TextureDesc& desc)
    {
        m_Handle     = TextureHandle{ { {}, Ptr{ initData }, desc } };
        m_IsImported = false;
    }

    //

    [[nodiscard]] static size_t ComputeViewHash(
        const BufferResourceViewDesc& viewDesc)
    {
        return std::visit(
            VariantVisitor{
                [](Dg::BUFFER_VIEW_TYPE type) -> size_t
                {
                    return static_cast<size_t>(type);
                },
                [](const Dg::BufferViewDesc& desc) -> size_t
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
                [](Dg::TEXTURE_VIEW_TYPE type) -> size_t
                {
                    return static_cast<size_t>(type);
                },
                [](const auto& desc) -> size_t
                {
                    using viewType = std::decay_t<decltype(desc)>;
                    return Dg::ComputeHash(static_cast<const Dg::TextureViewDesc&>(desc));
                } },
            viewDesc);
    }

    //

    size_t ResourceHandle::CreateView(
        const BufferResourceViewDesc& viewDesc)
    {
        size_t viewId = ComputeViewHash(viewDesc);

        Log::Gfx().Assert(std::holds_alternative<BufferHandle>(m_Handle), "Resource is not a buffer");
        auto& bufferHandle = std::get<BufferHandle>(m_Handle);
        if (!bufferHandle.Views.contains(viewId))
        {
            bufferHandle.Views.emplace(viewId, BufferResourceView{ {}, viewDesc });
        }

        return viewId;
    }

    size_t ResourceHandle::CreateView(
        const TextureResourceViewDesc& viewDesc)
    {
        size_t viewId = ComputeViewHash(viewDesc);

        Log::Gfx().Assert(std::holds_alternative<TextureHandle>(m_Handle), "Resource is not a texture");
        auto& textureHandle = std::get<TextureHandle>(m_Handle);
        if (!textureHandle.Views.contains(viewId))
        {
            textureHandle.Views.emplace(viewId, TextureResourceView{ {}, viewDesc });
        }

        return viewId;
    }

    //

    void ResourceHandle::Reallocate(
        Dg::IRenderDevice* renderDevice)
    {
        std::visit(
            VariantVisitor{
                [&](auto& handle)
                {
                    if (!IsImported())
                    {
                        using descType = std::decay_t<decltype(handle.Desc)>;
                        if (!handle.Resource || handle.Resource->GetDesc() != handle.Desc)
                        {
#ifndef AME_DIST
                            handle.Desc.Name = m_Name.c_str();
#endif
                            handle.Resource.Release();
                            if constexpr (std::is_same_v<descType, Dg::TextureDesc>)
                            {
                                if (handle.InitData)
                                {
                                    auto initData = handle.InitData->GetInitData();
                                    renderDevice->CreateTexture(handle.Desc, &initData, &handle.Resource);
                                }
                                else
                                {
                                    renderDevice->CreateTexture(handle.Desc, nullptr, &handle.Resource);
                                }
                            }
                            else
                            {
                                if (handle.InitData)
                                {
                                    auto initData = handle.InitData->GetInitData();
                                    renderDevice->CreateBuffer(handle.Desc, &initData, &handle.Resource);
                                }
                                else
                                {
                                    renderDevice->CreateBuffer(handle.Desc, nullptr, &handle.Resource);
                                }
                            }
                        }
                    }

                    RecreateViewsIfNeeded(handle);
                } },
            m_Handle);
    }

    //

    void ResourceHandle::RecreateViewsIfNeeded(
        BufferHandle& handle)
    {
        for (auto& viewHandle : handle.Views | std::views::values)
        {
            if (!viewHandle.View) [[unlikely]]
            {
                std::visit(
                    VariantVisitor{
                        [&](Dg::BUFFER_VIEW_TYPE type)
                        {
                            viewHandle.View = handle.Resource->GetDefaultView(type);
                        },
                        [&](const Dg::BufferViewDesc& desc)
                        {
                            handle.Resource->CreateView(desc, &viewHandle.View);
                        } },
                    viewHandle.Desc);
            }
        }
    }

    void ResourceHandle::RecreateViewsIfNeeded(
        TextureHandle& handle)
    {
        for (auto& viewHandle : handle.Views | std::views::values)
        {
            if (!viewHandle.View) [[unlikely]]
            {
                std::visit(
                    VariantVisitor{
                        [&](Dg::TEXTURE_VIEW_TYPE type)
                        {
                            viewHandle.View = handle.Resource->GetDefaultView(type);
                        },
                        [&](const auto& desc)
                        {
                            handle.Resource->CreateView(static_cast<const Dg::TextureViewDesc&>(desc), &viewHandle.View);
                        } },
                    viewHandle.Desc);
            }
        }
    }
} // namespace Ame::Rg