#pragma once

#include <Rhi/Descs/Core.hpp>

namespace Ame::Rhi
{
    using IndexType        = nri::IndexType;
    using BufferUsageBits  = nri::BufferUsageBits;
    using TextureType      = nri::TextureType;
    using TextureUsageBits = nri::TextureUsageBits;

    //

    static TextureDesc constexpr Tex1D(
        ResourceFormat   Format,
        Dim_t            Width,
        Mip_t            MipNum      = 0,
        Dim_t            ArraySize   = 1,
        TextureUsageBits UsageMask   = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         SampleCount = 1)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_1D,
            .usageMask = UsageMask,
            .format    = Format,
            .width     = Width,
            .height    = 1,
            .depth     = 1,
            .mipNum    = MipNum,
            .arraySize = ArraySize,
            .sampleNum = SampleCount
        };
    }

    [[nodiscard]] static constexpr TextureDesc Tex2D(
        ResourceFormat   Format,
        Dim_t            Width,
        Dim_t            Height,
        Mip_t            MipNum      = 0,
        Dim_t            ArraySize   = 1,
        TextureUsageBits UsageMask   = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         SampleCount = 1)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_2D,
            .usageMask = UsageMask,
            .format    = Format,
            .width     = Width,
            .height    = Height,
            .depth     = 1,
            .mipNum    = MipNum,
            .arraySize = ArraySize,
            .sampleNum = SampleCount
        };
    }

    [[nodiscard]] static constexpr TextureDesc Tex3D(
        ResourceFormat   Format,
        Dim_t            Width,
        Dim_t            Height,
        Dim_t            Depth,
        Mip_t            MipNum    = 0,
        TextureUsageBits UsageMask = TextureUsageBits::SHADER_RESOURCE)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_3D,
            .usageMask = UsageMask,
            .format    = Format,
            .width     = Width,
            .height    = Height,
            .depth     = Depth,
            .mipNum    = MipNum
        };
    }

    [[nodiscard]] static constexpr TextureDesc TexCube(
        ResourceFormat   Format,
        Dim_t            Width,
        Dim_t            Height,
        Mip_t            MipNum      = 0,
        TextureUsageBits UsageMask   = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         SampleCount = 1)
    {
        return Tex2D(Format, Width, Height, MipNum, 6, UsageMask, SampleCount);
    }
} // namespace Ame::Rhi