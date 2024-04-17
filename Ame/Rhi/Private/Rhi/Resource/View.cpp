#include <Rhi/Resource/View.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    static nri::ResourceViewBits ConvertViewBits(
        TextureViewFlags Flags)
    {
        using namespace EnumBitOperators;

        nri::ResourceViewBits Bits = {};
        if ((Flags & TextureViewFlags::ReadOnlyDepth) != TextureViewFlags::None)
        {
            Bits |= nri::ResourceViewBits::READONLY_DEPTH;
        }
        if ((Flags & TextureViewFlags::ReadOnlyDepth) != TextureViewFlags::None)
        {
            Bits |= nri::ResourceViewBits::READONLY_DEPTH;
        }

        return Bits;
    }

    struct TexView1D
    {
        using ViewType = nri::Texture1DViewType;
        using DescType = nri::Texture1DViewDesc;

        static constexpr ViewType Convert(
            TextureViewType Type)
        {
            switch (Type)
            {
            case TextureViewType::ShaderResource1D:
                return ViewType::SHADER_RESOURCE_1D;
            case TextureViewType::ShaderResource1DArray:
                return ViewType::SHADER_RESOURCE_1D_ARRAY;
            case TextureViewType::UnorderedAccess1D:
                return ViewType::SHADER_RESOURCE_STORAGE_1D;
            case TextureViewType::UnorderedAccess1DArray:
                return ViewType::SHADER_RESOURCE_STORAGE_1D_ARRAY;
            case TextureViewType::RenderTarget1D:
                return ViewType::COLOR_ATTACHMENT;
            case TextureViewType::DepthStencil1D:
                return ViewType::DEPTH_STENCIL_ATTACHMENT;
            default:
                std::unreachable();
            }
        }

        static DescType Convert(
            nri::CoreInterface&    NriCore,
            nri::Texture&          Texture,
            const TextureViewDesc& Desc)
        {
            DescType NriDesc{
                .texture     = &Texture,
                .viewType    = Convert(Desc.Type),
                .format      = Desc.Format,
                .mipOffset   = Desc.Subresource.Mips.Offset,
                .mipNum      = Desc.Subresource.Mips.Count,
                .arrayOffset = Desc.Subresource.Array.Offset,
                .arraySize   = Desc.Subresource.Array.Count,
                .flags       = ConvertViewBits(Desc.Flags)
            };

            if (Desc.Subresource.Mips.Count == RemainingSize<Mip_t> ||
                Desc.Subresource.Array.Count == RemainingSize<Dim_t>)
            {
                auto& NriTextureDesc = NriCore.GetTextureDesc(Texture);
                if (Desc.Subresource.Mips.Count == RemainingSize<Mip_t>)
                {
                    NriDesc.mipNum = NriTextureDesc.mipNum - Desc.Subresource.Mips.Offset;
                }
                if (Desc.Subresource.Array.Count == RemainingSize<Dim_t>)
                {
                    NriDesc.arraySize = NriTextureDesc.arraySize - Desc.Subresource.Array.Offset;
                }
            }

            return NriDesc;
        }
    };

    struct TexView2D
    {
        using ViewType = nri::Texture2DViewType;
        using DescType = nri::Texture2DViewDesc;

        static constexpr ViewType Convert(
            TextureViewType Type)
        {
            switch (Type)
            {
            case TextureViewType::ShaderResource2D:
                return ViewType::SHADER_RESOURCE_2D;
            case TextureViewType::ShaderResource2DArray:
                return ViewType::SHADER_RESOURCE_2D_ARRAY;
            case TextureViewType::ShaderResource2DCube:
                return ViewType::SHADER_RESOURCE_CUBE;
            case TextureViewType::ShaderResource2DCubeArray:
                return ViewType::SHADER_RESOURCE_CUBE_ARRAY;
            case TextureViewType::UnorderedAccess2D:
                return ViewType::SHADER_RESOURCE_STORAGE_2D;
            case TextureViewType::UnorderedAccess2DArray:
                return ViewType::SHADER_RESOURCE_STORAGE_2D_ARRAY;
            case TextureViewType::RenderTarget2D:
                return ViewType::COLOR_ATTACHMENT;
            case TextureViewType::DepthStencil2D:
                return ViewType::DEPTH_STENCIL_ATTACHMENT;
            default:
                std::unreachable();
            }
        }

        static DescType Convert(
            nri::CoreInterface&    NriCore,
            nri::Texture&          Texture,
            const TextureViewDesc& Desc)
        {
            DescType NriDesc{
                .texture     = &Texture,
                .viewType    = Convert(Desc.Type),
                .format      = Desc.Format,
                .mipOffset   = Desc.Subresource.Mips.Offset,
                .mipNum      = Desc.Subresource.Mips.Count,
                .arrayOffset = Desc.Subresource.Array.Offset,
                .arraySize   = Desc.Subresource.Array.Count,
                .flags       = ConvertViewBits(Desc.Flags)
            };

            if (Desc.Subresource.Mips.Count == RemainingSize<Mip_t> ||
                Desc.Subresource.Array.Count == RemainingSize<Dim_t>)
            {
                auto& NriTextureDesc = NriCore.GetTextureDesc(Texture);
                if (Desc.Subresource.Mips.Count == RemainingSize<Mip_t>)
                {
                    NriDesc.mipNum = NriTextureDesc.mipNum - Desc.Subresource.Mips.Offset;
                }
                if (Desc.Subresource.Array.Count == RemainingSize<Dim_t>)
                {
                    NriDesc.arraySize = NriTextureDesc.arraySize - Desc.Subresource.Array.Offset;
                }
            }

            return NriDesc;
        }
    };

    struct TexView3D
    {
        using ViewType = nri::Texture3DViewType;
        using DescType = nri::Texture3DViewDesc;

        static constexpr ViewType Convert(
            TextureViewType Type)
        {
            switch (Type)
            {
            case TextureViewType::ShaderResource3D:
                return ViewType::SHADER_RESOURCE_3D;
            case TextureViewType::UnorderedAccess3D:
                return ViewType::SHADER_RESOURCE_STORAGE_3D;
            case TextureViewType::RenderTarget3D:
                return ViewType::COLOR_ATTACHMENT;
            default:
                std::unreachable();
            }
        }

        static DescType Convert(
            nri::CoreInterface&    NriCore,
            nri::Texture&          Texture,
            const TextureViewDesc& Desc)
        {
            DescType NriDesc{
                .texture     = &Texture,
                .viewType    = Convert(Desc.Type),
                .format      = Desc.Format,
                .mipOffset   = Desc.Subresource.Mips.Offset,
                .mipNum      = Desc.Subresource.Mips.Count,
                .sliceOffset = Desc.Subresource.Array.Offset,
                .sliceNum    = Desc.Subresource.Array.Count,
                .flags       = ConvertViewBits(Desc.Flags)
            };

            if (Desc.Subresource.Mips.Count == RemainingSize<Mip_t> ||
                Desc.Subresource.Array.Count == RemainingSize<Dim_t>)
            {
                auto& NriTextureDesc = NriCore.GetTextureDesc(Texture);
                if (Desc.Subresource.Mips.Count == RemainingSize<Mip_t>)
                {
                    NriDesc.mipNum = NriTextureDesc.mipNum - Desc.Subresource.Mips.Offset;
                }
                if (Desc.Subresource.Array.Count == RemainingSize<Dim_t>)
                {
                    NriDesc.sliceNum = NriTextureDesc.arraySize - Desc.Subresource.Array.Offset;
                }
            }

            return NriDesc;
        }
    };

    struct BufferView
    {
        using DescType = nri::BufferViewDesc;

        [[nodiscard]] static nri::BufferViewDesc Convert(
            nri::CoreInterface& NriCore,
            nri::Buffer&        Buffer,
            BufferViewDesc      Desc)
        {
            nri::BufferViewDesc NriDesc{
                .buffer = &Buffer,
                .format = Desc.Format,
                .offset = Desc.Range.Offset,
                .size   = Desc.Range.Size
            };

            if (Desc.Range == EntireBuffer)
            {
                NriDesc.offset = 0;
                NriDesc.size   = NriCore.GetBufferDesc(Buffer).size;
            }

            return NriDesc;
        }
    };

    //

    ResourceView::ResourceView(
        DeviceImpl*      RhiDevice,
        nri::Descriptor* Descriptor) :
        m_Device(RhiDevice),
        m_Descriptor(Descriptor)
    {
    }

    ResourceView::ResourceView(
        Extern,
        DeviceImpl&      RhiDevice,
        nri::Descriptor* Descriptor) :
        m_Device(&RhiDevice),
        m_Descriptor(Descriptor),
        m_Owning(false)
    {
    }

    ResourceView::ResourceView(
        Extern,
        Device&          RhiDevice,
        nri::Descriptor* Descriptor) :
        m_Device(&RhiDevice.GetImpl()),
        m_Descriptor(Descriptor),
        m_Owning(false)
    {
    }

    ResourceView::ResourceView(
        ResourceView&& Other) noexcept :
        m_Device(std::exchange(Other.m_Device, nullptr)),
        m_Descriptor(std::exchange(Other.m_Descriptor, nullptr)),
        m_Owning(std::exchange(Other.m_Owning, false))
    {
    }

    ResourceView& ResourceView::operator=(
        ResourceView&& Other) noexcept
    {
        if (this != &Other)
        {
            Release();

            m_Device     = std::exchange(Other.m_Device, nullptr);
            m_Descriptor = std::exchange(Other.m_Descriptor, nullptr);
            m_Owning     = std::exchange(Other.m_Owning, false);
        }

        return *this;
    }

    ResourceView::~ResourceView()
    {
        Release();
    }

    void ResourceView::SetName(
        const char* Name) const
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetDescriptorDebugName(*m_Descriptor, Name);
    }

    nri::Descriptor* ResourceView::Unwrap() const
    {
        return m_Descriptor;
    }

    void* ResourceView::GetNative() const
    {
        auto& Nri     = m_Device->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        return std::bit_cast<void*>(NriCore.GetDescriptorNativeObject(*m_Descriptor));
    }

    //

    nri::Descriptor* DeviceImpl::CreateView(
        nri::Texture&          NriTexture,
        const TextureViewDesc& Desc) const
    {
        auto& NriCore = *m_NRI.GetCoreInterface();

        using namespace EnumBitOperators;
        nri::Descriptor* View = nullptr;
        if ((Desc.Type & TextureViewType::AnyOneDimensional) != TextureViewType::None)
        {
            auto NriDesc = TexView1D::Convert(NriCore, NriTexture, Desc);
            ThrowIfFailed(NriCore.CreateTexture1DView(NriDesc, View), "Failed to create texture view");
        }
        else if ((Desc.Type & TextureViewType::AnyTwoDimensional) != TextureViewType::None)
        {
            auto NriDesc = TexView2D::Convert(NriCore, NriTexture, Desc);
            ThrowIfFailed(NriCore.CreateTexture2DView(NriDesc, View), "Failed to create texture view");
        }
        else if ((Desc.Type & TextureViewType::AnyThreeDimensional) != TextureViewType::None)
        {
            auto NriDesc = TexView3D::Convert(NriCore, NriTexture, Desc);
            ThrowIfFailed(NriCore.CreateTexture3DView(NriDesc, View), "Failed to create texture view");
        }
        return View;
    }

    nri::Descriptor* DeviceImpl::CreateView(
        nri::Buffer&          NriBuffer,
        const BufferViewDesc& Desc) const
    {
        auto& NriCore = *m_NRI.GetCoreInterface();
        auto  NriDesc = BufferView::Convert(NriCore, NriBuffer, Desc);

        nri::Descriptor* View = nullptr;
        ThrowIfFailed(NriCore.CreateBufferView(NriDesc, View), "Failed to create buffer view");

        return View;
    }

    //

    BufferRange BufferRange::Transform(
        Buffer& RhiBuffer) const noexcept
    {
        auto Copy = *this;
        if (Copy.Size == RemainingSize<size_t>)
        {
            Copy.Size = RhiBuffer.GetDesc().size - Copy.Offset;
        }
        return Copy;
    }

    MipLevel MipLevel::Transform(
        Texture& RhiTexture) const noexcept
    {
        auto Copy = *this;
        if (Copy.Count == RemainingSize<Mip_t>)
        {
            Copy.Count = RhiTexture.GetDesc().mipNum - Copy.Offset;
        }
        return Copy;
    }

    ArraySlice ArraySlice::Transform(
        Texture& RhiTexture) const noexcept
    {
        auto Copy = *this;
        if (Copy.Count == RemainingSize<Dim_t>)
        {
            Copy.Count = RhiTexture.GetDesc().arraySize - Copy.Offset;
        }
        return Copy;
    }

    TextureSubresource TextureSubresource::Transform(
        Texture& RhiTexture) const noexcept
    {
        return {
            Mips.Transform(RhiTexture),
            Array.Transform(RhiTexture)
        };
    }

    //

    void DeviceImpl::Release(
        nri::Descriptor& NriDescriptor)
    {
        m_FrameManager.DeferRelease(NriDescriptor);
    }

    void Device::Release(
        nri::Descriptor& View)
    {
        m_Impl->Release(View);
    }

    void ResourceView::Release()
    {
        if (!m_Owning || !m_Device)
        {
            return;
        }

        m_Device->Release(*m_Descriptor);
        m_Descriptor = nullptr;
        m_Owning     = false;
    }
} // namespace Ame::Rhi