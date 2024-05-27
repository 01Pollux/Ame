#pragma once

#include <Rhi/Descs/Core.hpp>
#include <Math/Rect.hpp>

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
    static constexpr Ty c_RemainingSize = Ty{};

    struct BufferRange
    {
        size_t Offset = 0;
        size_t Size   = 0;

        constexpr BufferRange(
            size_t Offset = 0,
            size_t Size   = c_RemainingSize<size_t>) :
            Offset(Offset),
            Size(Size)
        {
        }

        auto operator<=>(
            const BufferRange&) const noexcept = default;

        [[nodiscard]] BufferRange Transform(
            const Buffer& buffer) const noexcept;
    };

    static constexpr BufferRange c_EntireBuffer;

    //

    struct MipLevel
    {
        Mip_t Offset = 0;
        Mip_t Count  = 0;

        constexpr MipLevel(
            Mip_t offset = 0,
            Mip_t count  = c_RemainingSize<Mip_t>) :
            Offset(offset),
            Count(count)
        {
        }

        auto operator<=>(
            const MipLevel&) const noexcept = default;

        [[nodiscard]] MipLevel Transform(
            const Texture& texture) const noexcept;
    };

    static constexpr MipLevel c_EntireMipChain;

    //

    struct ArraySlice
    {
        Dim_t Offset;
        Dim_t Count;

        constexpr ArraySlice(
            Dim_t Offset = 0,
            Dim_t Count  = c_RemainingSize<Dim_t>) :
            Offset(Offset),
            Count(Count)
        {
        }

        auto operator<=>(
            const ArraySlice&) const noexcept = default;

        [[nodiscard]] ArraySlice Transform(
            const Texture& texture) const noexcept;
    };

    static constexpr ArraySlice c_EntireArray;

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

    static constexpr TextureSubresource c_AllSubresources;

    //

    struct TextureRect
    {
        using Coordinate = std::array<Dim_t, 3>;

        Coordinate Position;
        Coordinate Size;

        constexpr TextureRect(
            Coordinate position = c_RemainingSize<Coordinate>,
            Coordinate size     = c_RemainingSize<Coordinate>) :
            Position(position),
            Size(size)
        {
        }

        auto operator<=>(
            const TextureRect&) const noexcept = default;

        [[nodiscard]] TextureRect Transform(
            const Texture& texture) const noexcept;
    };

    static constexpr TextureRect c_EntireTexture;

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