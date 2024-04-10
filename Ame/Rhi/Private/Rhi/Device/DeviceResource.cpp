#include <Rhi/Device.hpp>
#include "DeviceImpl.hpp"

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

    struct BufferView
    {
        using ViewType = nri::Texture1DViewType;
        using DescType = nri::Texture1DViewDesc;

        [[nodiscard]] static nri::BufferViewDesc Convert(
            nri::CoreInterface&   NriCore,
            nri::Buffer&          Buffer,
            const BufferViewDesc& Desc)
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

    nri::Texture* Device::Create(
        const TextureDesc& Desc)
    {
        nri::Texture* Tex = nullptr;
        auto&         Nri = m_Impl->GetNRI();
        ThrowIfFailed(Nri.GetCoreInterface()->CreateTexture(m_Impl->GetDevice(), Desc, Tex), "Failed to create texture");
        m_Impl->BeginTracking(Tex, { nri::AccessBits::UNKNOWN, nri::Layout::UNKNOWN, nri::StageBits::ALL });
        return Tex;
    }

    void Device::Release(
        nri::Texture& Tex,
        bool          Defer)
    {
        m_Impl->EndTracking(&Tex);
        if (Defer)
        {
            auto& Nri = m_Impl->GetNRI();
            Nri.GetCoreInterface()->DestroyTexture(Tex);
        }
        else
        {
            m_Impl->DeferRelease(Tex);
        }
    }

    void Device::SetName(
        nri::Texture& Tex,
        const char*   Name)
    {
        auto& Nri = m_Impl->GetNRI();
        Nri.GetCoreInterface()->SetTextureDebugName(Tex, Name);
    }

    const TextureDesc& Device::GetDesc(
        nri::Texture& Tex) const
    {
        auto& Nri = m_Impl->GetNRI();
        return Nri.GetCoreInterface()->GetTextureDesc(Tex);
    }

    void* Device::GetNative(
        nri::Texture& Tex) const
    {
        auto& Nri = m_Impl->GetNRI();
        return std::bit_cast<void*>(Nri.GetCoreInterface()->GetTextureNativeObject(Tex));
    }

    nri::Descriptor* Device::CreateView(
        nri::Texture&          Tex,
        const TextureViewDesc& Desc) const
    {
        auto& Nri     = m_Impl->GetNRI();
        auto  NriCore = Nri.GetCoreInterface();

        using namespace EnumBitOperators;
        nri::Descriptor* View = nullptr;
        if ((Desc.Type & TextureViewType::AnyOneDimensional) != TextureViewType::None)
        {
            auto NriDesc = TexView1D::Convert(*NriCore, Tex, Desc);
            ThrowIfFailed(NriCore->CreateTexture1DView(NriDesc, View), "Failed to create texture view");
        }
        else if ((Desc.Type & TextureViewType::AnyTwoDimensional) != TextureViewType::None)
        {
            auto NriDesc = TexView2D::Convert(*NriCore, Tex, Desc);
            ThrowIfFailed(NriCore->CreateTexture2DView(NriDesc, View), "Failed to create texture view");
        }
        else if ((Desc.Type & TextureViewType::AnyThreeDimensional) != TextureViewType::None)
        {
            auto NriDesc = TexView3D::Convert(*NriCore, Tex, Desc);
            ThrowIfFailed(NriCore->CreateTexture3DView(NriDesc, View), "Failed to create texture view");
        }
        return View;
    }

    //

    nri::Buffer* Device::Create(
        const BufferDesc& Desc)
    {
        nri::Buffer* Buf = nullptr;
        auto&        Nri = m_Impl->GetNRI();
        ThrowIfFailed(Nri.GetCoreInterface()->CreateBuffer(m_Impl->GetDevice(), Desc, Buf), "Failed to create buffer");
        m_Impl->BeginTracking(Buf, { nri::AccessBits::UNKNOWN, nri::StageBits::ALL });
        return Buf;
    }

    void Device::Release(
        nri::Buffer& Buf,
        bool         Defer)
    {
        m_Impl->EndTracking(&Buf);
        if (Defer)
        {
            auto& Nri = m_Impl->GetNRI();
            Nri.GetCoreInterface()->DestroyBuffer(Buf);
        }
        else
        {
            m_Impl->DeferRelease(Buf);
        }
    }

    void Device::SetName(
        nri::Buffer& Buf,
        const char*  Name)
    {
        auto& Nri = m_Impl->GetNRI();
        Nri.GetCoreInterface()->SetBufferDebugName(Buf, Name);
    }

    const BufferDesc& Device::GetDesc(
        nri::Buffer& Buf) const
    {
        auto& Nri = m_Impl->GetNRI();
        return Nri.GetCoreInterface()->GetBufferDesc(Buf);
    }

    void* Device::GetNative(
        nri::Buffer& Buf) const
    {
        auto& Nri = m_Impl->GetNRI();
        return std::bit_cast<void*>(Nri.GetCoreInterface()->GetBufferNativeObject(Buf));
    }

    nri::Descriptor* Device::CreateView(
        nri::Buffer&          Buf,
        const BufferViewDesc& Desc) const
    {
        auto& Nri     = m_Impl->GetNRI();
        auto  NriCore = Nri.GetCoreInterface();
        auto  NriDesc = BufferView::Convert(*NriCore, Buf, Desc);

        nri::Descriptor* View = nullptr;
        ThrowIfFailed(NriCore->CreateBufferView(NriDesc, View), "Failed to create buffer view");
        return View;
    }

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