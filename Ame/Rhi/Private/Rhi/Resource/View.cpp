#include <Rhi/Resource/View.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    static nri::ResourceViewBits ConvertViewBits(
        TextureViewFlags flags)
    {
        using namespace EnumBitOperators;

        nri::ResourceViewBits bits = {};
        if ((flags & TextureViewFlags::ReadOnlyDepth) != TextureViewFlags::None)
        {
            bits |= nri::ResourceViewBits::READONLY_DEPTH;
        }
        if ((flags & TextureViewFlags::ReadOnlyDepth) != TextureViewFlags::None)
        {
            bits |= nri::ResourceViewBits::READONLY_DEPTH;
        }

        return bits;
    }

    struct TexView1D
    {
        using ViewType = nri::Texture1DViewType;
        using DescType = nri::Texture1DViewDesc;

        static constexpr ViewType Convert(
            TextureViewType type)
        {
            switch (type)
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
            nri::Texture&          nriTexture,
            const TextureViewDesc& desc)
        {
            return {
                .texture     = &nriTexture,
                .viewType    = Convert(desc.Type),
                .format      = desc.Format,
                .mipOffset   = desc.Subresource.Mips.Offset,
                .mipNum      = desc.Subresource.Mips.Count,
                .arrayOffset = desc.Subresource.Array.Offset,
                .arraySize   = desc.Subresource.Array.Count,
                .flags       = ConvertViewBits(desc.Flags)
            };
        }
    };

    struct TexView2D
    {
        using ViewType = nri::Texture2DViewType;
        using DescType = nri::Texture2DViewDesc;

        static constexpr ViewType Convert(
            TextureViewType type)
        {
            switch (type)
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
            nri::Texture&          nriTexture,
            const TextureViewDesc& desc)
        {
            return {
                .texture     = &nriTexture,
                .viewType    = Convert(desc.Type),
                .format      = desc.Format,
                .mipOffset   = desc.Subresource.Mips.Offset,
                .mipNum      = desc.Subresource.Mips.Count,
                .arrayOffset = desc.Subresource.Array.Offset,
                .arraySize   = desc.Subresource.Array.Count,
                .flags       = ConvertViewBits(desc.Flags)
            };
        }
    };

    struct TexView3D
    {
        using ViewType = nri::Texture3DViewType;
        using DescType = nri::Texture3DViewDesc;

        static constexpr ViewType Convert(
            TextureViewType type)
        {
            switch (type)
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
            nri::Texture&          nriTexture,
            const TextureViewDesc& desc)
        {
            return {
                .texture     = &nriTexture,
                .viewType    = Convert(desc.Type),
                .format      = desc.Format,
                .mipOffset   = desc.Subresource.Mips.Offset,
                .mipNum      = desc.Subresource.Mips.Count,
                .sliceOffset = desc.Subresource.Array.Offset,
                .sliceNum    = desc.Subresource.Array.Count,
                .flags       = ConvertViewBits(desc.Flags)
            };
        }
    };

    struct BufferView
    {
        using DescType = nri::BufferViewDesc;
        using ViewType = nri::BufferViewType;

        static constexpr ViewType Convert(
            BufferViewType type)
        {
            switch (type)
            {
            case BufferViewType::ConstantBuffer:
                return ViewType::CONSTANT;
            case BufferViewType::ShaderResource:
                return ViewType::SHADER_RESOURCE;
            case BufferViewType::UnorderedAccess:
                return ViewType::SHADER_RESOURCE_STORAGE;
            default:
                std::unreachable();
            }
        }
        [[nodiscard]] static nri::BufferViewDesc Convert(
            nri::Buffer&          nriBuffer,
            const BufferViewDesc& desc)
        {
            return {
                .buffer   = &nriBuffer,
                .viewType = Convert(desc.Type),
                .format   = desc.Format,
                .offset   = desc.Range.Offset,
                .size     = desc.Range.Size,
            };
        }
    };

    //

    ResourceView::ResourceView(
        DeviceImpl*      rhiDevice,
        nri::Descriptor* nriDescriptor) :
        m_Device(rhiDevice),
        m_Descriptor(nriDescriptor)
    {
    }

    ResourceView::ResourceView(
        Device&            rhiDevice,
        const SamplerDesc& desc) :
        m_Device(&rhiDevice.GetImpl())
    {
        auto& rhiDeviceImpl = rhiDevice.GetImpl();
        auto& nriUtils      = rhiDeviceImpl.GetNRI();
        auto& nriCore       = *nriUtils.GetCoreInterface();

        ThrowIfFailed(nriCore.CreateSampler(m_Device->GetDevice(), desc, m_Descriptor), "Failed to create sampler");
    }

    ResourceView::ResourceView(
        Extern,
        DeviceImpl&      rhiDeviceImpl,
        nri::Descriptor* nriDescriptor) :
        m_Device(&rhiDeviceImpl),
        m_Descriptor(nriDescriptor),
        m_Owning(false)
    {
    }

    ResourceView::ResourceView(
        Extern,
        Device&          rhiDevice,
        nri::Descriptor* nriDescriptor) :
        m_Device(&rhiDevice.GetImpl()),
        m_Descriptor(nriDescriptor),
        m_Owning(false)
    {
    }

    ResourceView::ResourceView(
        const ResourceView& other) :
        m_Device(other.m_Device),
        m_Descriptor(other.m_Descriptor),
        m_Owning(false)
    {
    }

    ResourceView::ResourceView(
        ResourceView&& other) noexcept :
        m_Device(std::exchange(other.m_Device, nullptr)),
        m_Descriptor(std::exchange(other.m_Descriptor, nullptr)),
        m_Owning(std::exchange(other.m_Owning, false))
    {
    }

    ResourceView& ResourceView::operator=(
        const ResourceView& other)
    {
        if (this != &other)
        {
            Release();

            m_Device     = other.m_Device;
            m_Descriptor = other.m_Descriptor;
            m_Owning     = false;
        }

        return *this;
    }

    ResourceView& ResourceView::operator=(
        ResourceView&& other) noexcept
    {
        if (this != &other)
        {
            Release();

            m_Device     = std::exchange(other.m_Device, nullptr);
            m_Descriptor = std::exchange(other.m_Descriptor, nullptr);
            m_Owning     = std::exchange(other.m_Owning, false);
        }

        return *this;
    }

    ResourceView::~ResourceView()
    {
        Release();
    }

    void ResourceView::SetName(
        const char* name) const
    {
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.SetDescriptorDebugName(*m_Descriptor, name);
    }

    nri::Descriptor* ResourceView::Unwrap() const
    {
        return m_Descriptor;
    }

    void* ResourceView::GetNative() const
    {
        auto& nriUtils = m_Device->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        return std::bit_cast<void*>(nriCore.GetDescriptorNativeObject(*m_Descriptor));
    }

    bool ResourceView::IsOwning() const
    {
        return m_Owning;
    }

    //

    nri::Descriptor* DeviceImpl::CreateView(
        nri::Texture&          nriTexture,
        const TextureViewDesc& desc) const
    {
        using namespace EnumBitOperators;

        auto& nriCore = *m_NRI.GetCoreInterface();

        nri::Descriptor* nriDescriptor = nullptr;
        if ((desc.Type & TextureViewType::AnyOneDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView1D::Convert(nriTexture, desc);
            ThrowIfFailed(nriCore.CreateTexture1DView(nriDesc, nriDescriptor), "Failed to create texture view");
        }
        else if ((desc.Type & TextureViewType::AnyTwoDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView2D::Convert(nriTexture, desc);
            ThrowIfFailed(nriCore.CreateTexture2DView(nriDesc, nriDescriptor), "Failed to create texture view");
        }
        else if ((desc.Type & TextureViewType::AnyThreeDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView3D::Convert(nriTexture, desc);
            ThrowIfFailed(nriCore.CreateTexture3DView(nriDesc, nriDescriptor), "Failed to create texture view");
        }

        return nriDescriptor;
    }

    nri::Descriptor* DeviceImpl::CreateView(
        nri::Buffer&          NriBuffer,
        const BufferViewDesc& Desc) const
    {
        auto& nriCore = *m_NRI.GetCoreInterface();

        nri::Descriptor* nriDescriptor = nullptr;
        auto             nriDesc       = BufferView::Convert(NriBuffer, Desc);
        ThrowIfFailed(nriCore.CreateBufferView(nriDesc, nriDescriptor), "Failed to create buffer view");

        return nriDescriptor;
    }

    //

    BufferRange BufferRange::Transform(
        const Buffer& buffer) const noexcept
    {
        auto range = *this;
        if (range.Size == RemainingSize<size_t>)
        {
            range.Size = buffer.GetDesc().size - range.Offset;
        }
        return range;
    }

    MipLevel MipLevel::Transform(
        const Texture& texture) const noexcept
    {
        auto mipLevel = *this;
        if (mipLevel.Count == RemainingSize<Mip_t>)
        {
            mipLevel.Count = texture.GetDesc().mipNum - mipLevel.Offset;
        }
        return mipLevel;
    }

    ArraySlice ArraySlice::Transform(
        const Texture& texture) const noexcept
    {
        auto arraySlice = *this;
        if (arraySlice.Count == RemainingSize<Dim_t>)
        {
            arraySlice.Count = texture.GetDesc().arraySize - arraySlice.Offset;
        }
        return arraySlice;
    }

    TextureSubresource TextureSubresource::Transform(
        const Texture& texture) const noexcept
    {
        return {
            Mips.Transform(texture),
            Array.Transform(texture)
        };
    }

    //

    BufferViewDesc BufferViewDesc::Transform(
        const Buffer& buffer) const noexcept
    {
        auto viewDesc  = *this;
        viewDesc.Range = Range.Transform(buffer);
        return viewDesc;
    }

    TextureViewDesc TextureViewDesc::Transform(
        const Texture& texture) const noexcept
    {
        auto viewDesc        = *this;
        viewDesc.Subresource = Subresource.Transform(texture);
        if (Format == Rhi::ResourceFormat::UNKNOWN)
        {
            viewDesc.Format = texture.GetDesc().format;
        }
        return viewDesc;
    }

    //

    void DeviceImpl::Release(
        nri::Descriptor& nriDescriptor)
    {
        m_FrameManager.DeferRelease(nriDescriptor);
    }

    void Device::Release(
        nri::Descriptor& nriDescriptor)
    {
        m_Impl->Release(nriDescriptor);
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
    };

    //

    SamplerResourceView::SamplerResourceView(
        Extern,
        DeviceImpl&      rhiDeviceImpl,
        nri::Descriptor* nriDescriptor) :
        ResourceView({}, rhiDeviceImpl, nriDescriptor)
    {
    }

    SamplerResourceView::SamplerResourceView(
        Extern,
        Device&          rhiDevice,
        nri::Descriptor* nriDescriptor) :
        ResourceView({}, rhiDevice, nriDescriptor)
    {
    }

    SamplerResourceView::SamplerResourceView(
        Device&            rhiDevice,
        const SamplerDesc& desc) :
        ResourceView(rhiDevice, desc)
    {
    }
} // namespace Ame::Rhi