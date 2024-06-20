#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/Device.Impl.hpp>

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

    Co::result<ResourceView> DeviceResourceAllocator::CreateView(
        const Buffer&         buffer,
        const BufferViewDesc& desc)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriDevice        = rhiDeviceWrapper.GetNriDevice();
        auto& nriCore          = *nri.GetCoreInterface();

        nri::Descriptor* descriptor = nullptr;
        auto             nriDesc    = BufferView::Convert(*buffer.Unwrap(), desc);
        ThrowIfFailed(nriCore.CreateBufferView(nriDesc, descriptor), "Failed to create buffer view");

        co_return ResourceView(nriCore, descriptor);
    }

    Co::result<ResourceView> DeviceResourceAllocator::CreateView(
        const Texture&         texture,
        const TextureViewDesc& desc)
    {
        using namespace EnumBitOperators;

        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriDevice        = rhiDeviceWrapper.GetNriDevice();
        auto& nriCore          = *nri.GetCoreInterface();

        nri::Descriptor* descriptor = nullptr;
        if ((desc.Type & TextureViewType::AnyOneDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView1D::Convert(*texture.Unwrap(), desc);
            ThrowIfFailed(nriCore.CreateTexture1DView(nriDesc, descriptor), "Failed to create texture view");
        }
        else if ((desc.Type & TextureViewType::AnyTwoDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView2D::Convert(*texture.Unwrap(), desc);
            ThrowIfFailed(nriCore.CreateTexture2DView(nriDesc, descriptor), "Failed to create texture view");
        }
        else if ((desc.Type & TextureViewType::AnyThreeDimensional) != TextureViewType::None)
        {
            auto nriDesc = TexView3D::Convert(*texture.Unwrap(), desc);
            ThrowIfFailed(nriCore.CreateTexture3DView(nriDesc, descriptor), "Failed to create texture view");
        }

        co_return ResourceView(nriCore, descriptor);
    }

    Co::result<ResourceView> DeviceResourceAllocator::CreateSampler(
        const SamplerDesc& desc)
    {
        auto& rhiDeviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri              = rhiDeviceWrapper.GetNri();
        auto& nriDevice        = rhiDeviceWrapper.GetNriDevice();
        auto& nriCore          = *nri.GetCoreInterface();

        nri::Descriptor* descriptor = nullptr;
        ThrowIfFailed(nriCore.CreateSampler(nriDevice, desc, descriptor), "Failed to create sampler");

        co_return ResourceView(nriCore, descriptor);
    }

    //

    Co::result<void> DeviceResourceAllocator::Release(
        ResourceView& view)
    {
        auto& frameManager = m_RhiDeviceImpl.get().GetFrameManager();
        auto& frame        = frameManager.GetCurrentFrame();

        auto lock = co_await TryLock();
        frame.DeferRelease(*view.Unwrap());
    }
} // namespace Ame::Rhi