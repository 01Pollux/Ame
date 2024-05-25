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
        ResourceFormat   format,
        Dim_t            width,
        Mip_t            mipNum      = 0,
        Dim_t            arraySize   = 1,
        TextureUsageBits usageMask   = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         sampleCount = 1)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_1D,
            .usageMask = usageMask,
            .format    = format,
            .width     = width,
            .height    = 1,
            .depth     = 1,
            .mipNum    = mipNum,
            .arraySize = arraySize,
            .sampleNum = sampleCount
        };
    }

    [[nodiscard]] static constexpr TextureDesc Tex2D(
        ResourceFormat   format,
        Dim_t            width,
        Dim_t            height,
        Mip_t            mipCount    = 0,
        Dim_t            arraySize   = 1,
        TextureUsageBits usageMask   = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         sampleCount = 1)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_2D,
            .usageMask = usageMask,
            .format    = format,
            .width     = width,
            .height    = height,
            .depth     = 1,
            .mipNum    = mipCount,
            .arraySize = arraySize,
            .sampleNum = sampleCount
        };
    }

    [[nodiscard]] static constexpr TextureDesc Tex3D(
        ResourceFormat   format,
        Dim_t            width,
        Dim_t            height,
        Dim_t            depth,
        Mip_t            mipCount  = 0,
        TextureUsageBits usageMask = TextureUsageBits::SHADER_RESOURCE)
    {
        return TextureDesc{
            .type      = TextureType::TEXTURE_3D,
            .usageMask = usageMask,
            .format    = format,
            .width     = width,
            .height    = height,
            .depth     = depth,
            .mipNum    = mipCount
        };
    }

    [[nodiscard]] static constexpr TextureDesc TexCube(
        ResourceFormat   format,
        Dim_t            width,
        Dim_t            height,
        Mip_t            mipCount    = 0,
        TextureUsageBits usageMask   = TextureUsageBits::SHADER_RESOURCE,
        Sample_t         sampleCount = 1)
    {
        return Tex2D(format, width, height, mipCount, 6, usageMask, sampleCount);
    }
} // namespace Ame::Rhi