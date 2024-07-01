#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

#include <Rhi/Descs/View.hpp>

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
            const Texture&         texture,
            const TextureViewDesc& viewDesc)
        {
            auto subresource = viewDesc.Subresource.Transform(texture);
            return {
                .texture     = texture.Unwrap(),
                .viewType    = Convert(viewDesc.Type),
                .format      = viewDesc.Format == ResourceFormat::UNKNOWN ? texture.GetDesc().format : viewDesc.Format,
                .mipOffset   = subresource.Mips.Offset,
                .mipNum      = subresource.Mips.Count,
                .arrayOffset = subresource.Array.Offset,
                .arraySize   = subresource.Array.Count,
                .flags       = ConvertViewBits(viewDesc.Flags)
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
            const Texture&         texture,
            const TextureViewDesc& viewDesc)
        {
            auto subresource = viewDesc.Subresource.Transform(texture);
            return {
                .texture     = texture.Unwrap(),
                .viewType    = Convert(viewDesc.Type),
                .format      = viewDesc.Format == ResourceFormat::UNKNOWN ? texture.GetDesc().format : viewDesc.Format,
                .mipOffset   = subresource.Mips.Offset,
                .mipNum      = subresource.Mips.Count,
                .arrayOffset = subresource.Array.Offset,
                .arraySize   = subresource.Array.Count,
                .flags       = ConvertViewBits(viewDesc.Flags)
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
            const Texture&         texture,
            const TextureViewDesc& viewDesc)
        {
            auto subresource = viewDesc.Subresource.Transform(texture);
            return {
                .texture     = texture.Unwrap(),
                .viewType    = Convert(viewDesc.Type),
                .format      = viewDesc.Format == ResourceFormat::UNKNOWN ? texture.GetDesc().format : viewDesc.Format,
                .mipOffset   = subresource.Mips.Offset,
                .mipNum      = subresource.Mips.Count,
                .sliceOffset = subresource.Array.Offset,
                .sliceNum    = subresource.Array.Count,
                .flags       = ConvertViewBits(viewDesc.Flags)
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
            const Buffer&         buffer,
            const BufferViewDesc& viewDesc)
        {
            auto range = viewDesc.Range.Transform(buffer);
            return {
                .buffer   = buffer.Unwrap(),
                .viewType = Convert(viewDesc.Type),
                .format   = viewDesc.Format,
                .offset   = range.Offset,
                .size     = range.Size,
            };
        }
    };

    //

    ResourceView Buffer::CreateView(
        const BufferViewDesc& viewDesc) const
    {
        auto& nriCore = m_Allocator->GetNriCore();

        nri::Descriptor* descriptor = nullptr;
        auto             nriDesc    = BufferView::Convert(*this, viewDesc);
        ThrowIfFailed(
            nriCore.CreateBufferView(nriDesc, descriptor),
            "Failed to create buffer view");

        return ResourceView(*m_Allocator, descriptor);
    }

    ResourceView Texture::CreateView(
        const TextureViewDesc& viewDesc) const
    {
        using namespace EnumBitOperators;

        auto& nriCore = m_Allocator->GetNriCore();

        nri::Descriptor* descriptor = nullptr;
        if ((viewDesc.Type & TextureViewType::AnyOneDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView1D::Convert(*this, viewDesc);
            ThrowIfFailed(
                nriCore.CreateTexture1DView(nriDesc, descriptor),
                "Failed to create texture view");
        }
        else if ((viewDesc.Type & TextureViewType::AnyTwoDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView2D::Convert(*this, viewDesc);
            ThrowIfFailed(
                nriCore.CreateTexture2DView(nriDesc, descriptor),
                "Failed to create texture view");
        }
        else if ((viewDesc.Type & TextureViewType::AnyThreeDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView3D::Convert(*this, viewDesc);
            ThrowIfFailed(
                nriCore.CreateTexture3DView(nriDesc, descriptor),
                "Failed to create texture view");
        }

        return ResourceView(*m_Allocator, descriptor);
    }

    ResourceView DeviceResourceAllocator::CreateSampler(
        const SamplerDesc& desc)
    {
        auto& deviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nriDevice     = deviceWrapper.GetNriDevice();
        auto& nriCore       = GetNriCore();

        nri::Descriptor* descriptor = nullptr;
        ThrowIfFailed(
            nriCore.CreateSampler(nriDevice, desc, descriptor),
            "Failed to create sampler");

        return ResourceView(*this, descriptor);
    }
} // namespace Ame::Rhi