#pragma once

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    enum class BufferViewType : uint8_t
    {
        ConstantBuffer,
        ShaderResource,
        UnorderedAccess,
    };

    enum class TextureViewType : uint32_t
    {
        None,

        ShaderResource1D       = 1 << 0,
        ShaderResource1DArray  = 1 << 1,
        UnorderedAccess1D      = 1 << 2,
        UnorderedAccess1DArray = 1 << 3,
        RenderTarget1D         = 1 << 4,
        DepthStencil1D         = 1 << 5,

        AnyOneDimensional =
            ShaderResource1D |
            ShaderResource1DArray |
            UnorderedAccess1D |
            UnorderedAccess1DArray |
            RenderTarget1D |
            DepthStencil1D,

        ShaderResource2D          = 1 << 6,
        ShaderResource2DArray     = 1 << 7,
        ShaderResource2DCube      = 1 << 8,
        ShaderResource2DCubeArray = 1 << 9,
        UnorderedAccess2D         = 1 << 10,
        UnorderedAccess2DArray    = 1 << 11,
        RenderTarget2D            = 1 << 12,
        DepthStencil2D            = 1 << 13,

        AnyTwoDimensional =
            ShaderResource2D |
            ShaderResource2DArray |
            ShaderResource2DCube |
            ShaderResource2DCubeArray |
            UnorderedAccess2D |
            UnorderedAccess2DArray |
            RenderTarget2D |
            DepthStencil2D,

        ShaderResource3D  = 1 << 14,
        UnorderedAccess3D = 1 << 15,
        RenderTarget3D    = 1 << 16,

        AnyThreeDimensional =
            ShaderResource3D |
            UnorderedAccess3D |
            RenderTarget3D,

        AnyShaderResource = ShaderResource1D |
                            ShaderResource1DArray |
                            ShaderResource2D |
                            ShaderResource2DArray |
                            ShaderResource2DCube |
                            ShaderResource2DCubeArray |
                            ShaderResource3D,

        AnyUnorderedAccess = UnorderedAccess1D |
                             UnorderedAccess1DArray |
                             UnorderedAccess2D |
                             UnorderedAccess2DArray |
                             UnorderedAccess3D,

        AnyRenderTarget = RenderTarget1D |
                          RenderTarget2D |
                          RenderTarget3D,

        AnyDepthStencil = DepthStencil1D |
                          DepthStencil2D
    };

    enum class TextureViewFlags : uint8_t
    {
        None            = 0,
        ReadOnlyDepth   = 1 << 0,
        ReadOnlyStencil = 1 << 1,

        ReadOnlyDepthStencil = ReadOnlyDepth | ReadOnlyStencil
    };

    //

    template<typename Ty>
    static constexpr Ty c_RemainingSize = static_cast<Ty>(0);

    struct BufferRange
    {
        size_t Offset = 0;
        size_t Size   = 0;

        constexpr BufferRange(
            size_t Offset = 0,
            size_t Size   = 0) :
            Offset(Offset),
            Size(Size)
        {
        }

        auto operator<=>(
            const BufferRange&) const noexcept = default;

        [[nodiscard]] BufferRange Transform(
            const Buffer& buffer) const noexcept;
    };

    static constexpr BufferRange c_EntireBuffer = BufferRange(0, c_RemainingSize<size_t>);

    //

    struct MipLevel
    {
        Mip_t Offset = 0;
        Mip_t Count  = 0;

        constexpr MipLevel(
            Mip_t offset = 0,
            Mip_t count  = 0) :
            Offset(offset),
            Count(count)
        {
        }

        auto operator<=>(
            const MipLevel&) const noexcept = default;

        [[nodiscard]] MipLevel Transform(
            const Texture& texture) const noexcept;
    };

    static constexpr MipLevel c_EntireMipChain = MipLevel(0, c_RemainingSize<Mip_t>);

    //

    struct ArraySlice
    {
        Dim_t Offset;
        Dim_t Count;

        constexpr ArraySlice(
            Dim_t Offset = 0,
            Dim_t Count  = 0) :
            Offset(Offset),
            Count(Count)
        {
        }

        auto operator<=>(
            const ArraySlice&) const noexcept = default;

        [[nodiscard]] ArraySlice Transform(
            const Texture& texture) const noexcept;
    };

    static constexpr ArraySlice c_EntireArray = ArraySlice(0, c_RemainingSize<Dim_t>);

    //

    struct TextureSubresource
    {
        MipLevel   Mips;
        ArraySlice Array;

        constexpr TextureSubresource(
            MipLevel   mips  = c_EntireMipChain,
            ArraySlice array = c_EntireArray) :
            Mips(mips),
            Array(array)
        {
        }

        auto operator<=>(
            const TextureSubresource&) const noexcept = default;

        [[nodiscard]] TextureSubresource Transform(
            const Texture& texture) const noexcept;
    };

    static constexpr TextureSubresource c_AllSubresources = TextureSubresource(c_EntireMipChain, c_EntireArray);

    //

    struct BufferViewDesc
    {
        BufferRange    Range  = c_EntireBuffer;
        ResourceFormat Format = ResourceFormat::UNKNOWN;
        BufferViewType Type   = BufferViewType::ConstantBuffer;

        [[nodiscard]] BufferViewDesc Transform(
            const Buffer& buffer) const noexcept;
    };

    struct TextureViewDesc
    {
        TextureViewType    Type;
        TextureSubresource Subresource = c_AllSubresources;
        ResourceFormat     Format      = ResourceFormat::UNKNOWN; // default to the format of the texture
        TextureViewFlags   Flags       = TextureViewFlags::None;

        [[nodiscard]] TextureViewDesc Transform(
            const Texture& texture) const noexcept;
    };
} // namespace Ame::Rhi