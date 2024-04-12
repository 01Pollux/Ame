#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/DeviceImpl.hpp>

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

            if (Desc.Subresource.Mips == EntireMipChain)
            {
                NriDesc.mipOffset = 0;
                NriDesc.mipNum    = NriCore.GetTextureDesc(Texture).mipNum;
            }
            if (Desc.Subresource.Array == EntireArray)
            {
                NriDesc.arrayOffset = 0;
                NriDesc.arraySize   = NriCore.GetTextureDesc(Texture).arraySize;
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

            if (Desc.Subresource.Mips == EntireMipChain)
            {
                NriDesc.mipOffset = 0;
                NriDesc.mipNum    = NriCore.GetTextureDesc(Texture).mipNum;
            }
            if (Desc.Subresource.Array == EntireArray)
            {
                NriDesc.arrayOffset = 0;
                NriDesc.arraySize   = NriCore.GetTextureDesc(Texture).arraySize;
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

            if (Desc.Subresource.Mips == EntireMipChain)
            {
                NriDesc.mipOffset = 0;
                NriDesc.mipNum    = NriCore.GetTextureDesc(Texture).mipNum;
            }
            if (Desc.Subresource.Array == EntireArray)
            {
                NriDesc.sliceOffset = 0;
                NriDesc.sliceNum    = NriCore.GetTextureDesc(Texture).arraySize;
            }

            return NriDesc;
        }
    };

    //


    //

    //

    void Device::Release(
        nri::Descriptor& View,
        bool             Defer)
    {
        if (Defer)
        {
            auto& Nri = m_Impl->GetNRI();
            Nri.GetCoreInterface()->DestroyDescriptor(View);
        }
        else
        {
            m_Impl->DeferRelease(View);
        }
    }

    void Device::SetName(
        nri::Descriptor& View,
        const char*      Name)
    {
        auto& Nri = m_Impl->GetNRI();
        Nri.GetCoreInterface()->SetDescriptorDebugName(View, Name);
    }

    void* Device::GetNative(
        nri::Descriptor& View) const
    {
        auto& Nri = m_Impl->GetNRI();
        return std::bit_cast<void*>(Nri.GetCoreInterface()->GetDescriptorNativeObject(View));
    }
} // namespace Ame::Rhi