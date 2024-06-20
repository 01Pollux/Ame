#ifdef AME_PLATFORM_WINDOWS
#include <Rhi/Device/Wrapper/D3D12/Resource.hpp>

#include <Math/Common.hpp>
#include "Resource.hpp"
#include "Resource.hpp"

namespace Ame::Rhi::D3D12
{
    // https://github.com/NVIDIAGameWorks/NRI/blob/main/Source/Shared/SharedExternal.cpp#L14
    static constexpr std::array<DxgiFormat, (size_t)nri::Format::MAX_NUM> c_DxgiFormatTable = { {
        { DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN },                            // UNKNOWN
        { DXGI_FORMAT_R8_TYPELESS, DXGI_FORMAT_R8_UNORM },                       // R8_UNORM
        { DXGI_FORMAT_R8_TYPELESS, DXGI_FORMAT_R8_SNORM },                       // R8_SNORM
        { DXGI_FORMAT_R8_TYPELESS, DXGI_FORMAT_R8_UINT },                        // R8_UINT
        { DXGI_FORMAT_R8_TYPELESS, DXGI_FORMAT_R8_SINT },                        // R8_SINT
        { DXGI_FORMAT_R8G8_TYPELESS, DXGI_FORMAT_R8G8_UNORM },                   // RG8_UNORM
        { DXGI_FORMAT_R8G8_TYPELESS, DXGI_FORMAT_R8G8_SNORM },                   // RG8_SNORM
        { DXGI_FORMAT_R8G8_TYPELESS, DXGI_FORMAT_R8G8_UINT },                    // RG8_UINT
        { DXGI_FORMAT_R8G8_TYPELESS, DXGI_FORMAT_R8G8_SINT },                    // RG8_SINT
        { DXGI_FORMAT_B8G8R8A8_TYPELESS, DXGI_FORMAT_B8G8R8A8_UNORM },           // BGRA8_UNORM
        { DXGI_FORMAT_B8G8R8A8_TYPELESS, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB },      // BGRA8_SRGB
        { DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM },           // RGBA8_UNORM
        { DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB },      // RGBA8_SRGB
        { DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_SNORM },           // RGBA8_SNORM
        { DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_UINT },            // RGBA8_UINT
        { DXGI_FORMAT_R8G8B8A8_TYPELESS, DXGI_FORMAT_R8G8B8A8_SINT },            // RGBA8_SINT
        { DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_R16_UNORM },                     // R16_UNORM
        { DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_R16_SNORM },                     // R16_SNORM
        { DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_R16_UINT },                      // R16_UINT
        { DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_R16_SINT },                      // R16_SINT
        { DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_R16_FLOAT },                     // R16_SFLOAT
        { DXGI_FORMAT_R16G16_TYPELESS, DXGI_FORMAT_R16G16_UNORM },               // RG16_UNORM
        { DXGI_FORMAT_R16G16_TYPELESS, DXGI_FORMAT_R16G16_SNORM },               // RG16_SNORM
        { DXGI_FORMAT_R16G16_TYPELESS, DXGI_FORMAT_R16G16_UINT },                // RG16_UINT
        { DXGI_FORMAT_R16G16_TYPELESS, DXGI_FORMAT_R16G16_SINT },                // RG16_SINT
        { DXGI_FORMAT_R16G16_TYPELESS, DXGI_FORMAT_R16G16_FLOAT },               // RG16_SFLOAT
        { DXGI_FORMAT_R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_UNORM },   // RGBA16_UNORM
        { DXGI_FORMAT_R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_SNORM },   // RGBA16_SNORM
        { DXGI_FORMAT_R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_UINT },    // RGBA16_UINT
        { DXGI_FORMAT_R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_SINT },    // RGBA16_SINT
        { DXGI_FORMAT_R16G16B16A16_TYPELESS, DXGI_FORMAT_R16G16B16A16_FLOAT },   // RGBA16_SFLOAT
        { DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_UINT },                      // R32_UINT
        { DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_SINT },                      // R32_SINT
        { DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_R32_FLOAT },                     // R32_SFLOAT
        { DXGI_FORMAT_R32G32_TYPELESS, DXGI_FORMAT_R32G32_UINT },                // RG32_UINT
        { DXGI_FORMAT_R32G32_TYPELESS, DXGI_FORMAT_R32G32_SINT },                // RG32_SINT
        { DXGI_FORMAT_R32G32_TYPELESS, DXGI_FORMAT_R32G32_FLOAT },               // RG32_SFLOAT
        { DXGI_FORMAT_R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_UINT },          // RGB32_UINT
        { DXGI_FORMAT_R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_SINT },          // RGB32_SINT
        { DXGI_FORMAT_R32G32B32_TYPELESS, DXGI_FORMAT_R32G32B32_FLOAT },         // RGB32_SFLOAT
        { DXGI_FORMAT_R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_UINT },    // RGBA32_UINT
        { DXGI_FORMAT_R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_SINT },    // RGBA32_SINT
        { DXGI_FORMAT_R32G32B32A32_TYPELESS, DXGI_FORMAT_R32G32B32A32_FLOAT },   // RGBA32_SFLOAT
        { DXGI_FORMAT_B5G6R5_UNORM, DXGI_FORMAT_B5G6R5_UNORM },                  // B5_G6_R5_UNORM
        { DXGI_FORMAT_B5G5R5A1_UNORM, DXGI_FORMAT_B5G5R5A1_UNORM },              // B5_G5_R5_A1_UNORM
        { DXGI_FORMAT_B4G4R4A4_UNORM, DXGI_FORMAT_B4G4R4A4_UNORM },              // B4_G4_R4_A4_UNORM
        { DXGI_FORMAT_R10G10B10A2_TYPELESS, DXGI_FORMAT_R10G10B10A2_UNORM },     // R10_G10_B10_A2_UNORM
        { DXGI_FORMAT_R10G10B10A2_TYPELESS, DXGI_FORMAT_R10G10B10A2_UINT },      // R10_G10_B10_A2_UINT
        { DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT },            // R11_G11_B10_UFLOAT
        { DXGI_FORMAT_R9G9B9E5_SHAREDEXP, DXGI_FORMAT_R9G9B9E5_SHAREDEXP },      // R9_G9_B9_E5_UFLOAT
        { DXGI_FORMAT_BC1_TYPELESS, DXGI_FORMAT_BC1_UNORM },                     // BC1_RGBA_UNORM
        { DXGI_FORMAT_BC1_TYPELESS, DXGI_FORMAT_BC1_UNORM_SRGB },                // BC1_RGBA_SRGB
        { DXGI_FORMAT_BC2_TYPELESS, DXGI_FORMAT_BC2_UNORM },                     // BC2_RGBA_UNORM
        { DXGI_FORMAT_BC2_TYPELESS, DXGI_FORMAT_BC2_UNORM_SRGB },                // BC2_RGBA_SRGB
        { DXGI_FORMAT_BC3_TYPELESS, DXGI_FORMAT_BC3_UNORM },                     // BC3_RGBA_UNORM
        { DXGI_FORMAT_BC3_TYPELESS, DXGI_FORMAT_BC3_UNORM_SRGB },                // BC3_RGBA_SRGB
        { DXGI_FORMAT_BC4_TYPELESS, DXGI_FORMAT_BC4_UNORM },                     // BC4_R_UNORM
        { DXGI_FORMAT_BC4_TYPELESS, DXGI_FORMAT_BC4_SNORM },                     // BC4_R_SNORM
        { DXGI_FORMAT_BC5_TYPELESS, DXGI_FORMAT_BC5_UNORM },                     // BC5_RG_UNORM
        { DXGI_FORMAT_BC5_TYPELESS, DXGI_FORMAT_BC5_SNORM },                     // BC5_RG_SNORM
        { DXGI_FORMAT_BC6H_TYPELESS, DXGI_FORMAT_BC6H_UF16 },                    // BC6H_RGB_UFLOAT
        { DXGI_FORMAT_BC6H_TYPELESS, DXGI_FORMAT_BC6H_SF16 },                    // BC6H_RGB_SFLOAT
        { DXGI_FORMAT_BC7_TYPELESS, DXGI_FORMAT_BC7_UNORM },                     // BC7_RGBA_UNORM
        { DXGI_FORMAT_BC7_TYPELESS, DXGI_FORMAT_BC7_UNORM_SRGB },                // BC7_RGBA_SRGB
        { DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_D16_UNORM },                     // D16_UNORM
        { DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT },           // D24_UNORM_S8_UINT
        { DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT },                     // D32_SFLOAT
        { DXGI_FORMAT_R32G8X24_TYPELESS, DXGI_FORMAT_D32_FLOAT_S8X24_UINT },     // D32_SFLOAT_S8_UINT_X24
        { DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS },       // R24_UNORM_X8
        { DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_X24_TYPELESS_G8_UINT },        // X24_G8_UINT
        { DXGI_FORMAT_R32G8X24_TYPELESS, DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS }, // R32_SFLOAT_X8_X24
        { DXGI_FORMAT_R32G8X24_TYPELESS, DXGI_FORMAT_X32_TYPELESS_G8X24_UINT },  // X32_G8_UINT_X24
    } };

    constexpr DxgiFormat DxgiFormat::Get(
        ResourceFormat format)
    {
        return c_DxgiFormatTable[static_cast<size_t>(format)];
    }

    //

    D3D12_RESOURCE_FLAGS GetBufferFlags(
        BufferUsageBits bufferUsageMask)
    {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

        if (bufferUsageMask & BufferUsageBits::SHADER_RESOURCE_STORAGE)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        return flags;
    };

    D3D12_RESOURCE_DESC GetResourceDesc(
        const BufferDesc& bufferDesc)
    {
        return D3D12_RESOURCE_DESC{
            .Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER,
            .Width            = bufferDesc.size,
            .Height           = 1,
            .DepthOrArraySize = 1,
            .MipLevels        = 1,
            .Format           = DXGI_FORMAT_UNKNOWN,
            .SampleDesc       = { 1, 0 },
            .Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
            .Flags            = GetBufferFlags(bufferDesc.usageMask)
        };
    }

    //

    D3D12_RESOURCE_FLAGS GetTextureFlags(
        TextureUsageBits textureUsageMask)
    {
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

        if (textureUsageMask & TextureUsageBits::SHADER_RESOURCE_STORAGE)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        if (textureUsageMask & TextureUsageBits::COLOR_ATTACHMENT)
            flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        if (textureUsageMask & TextureUsageBits::DEPTH_STENCIL_ATTACHMENT)
        {
            flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            if (!(textureUsageMask & TextureUsageBits::SHADER_RESOURCE))
                flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
        }

        return flags;
    };

    D3D12_RESOURCE_DIMENSION GetResourceDimension(
        TextureType textureType)
    {
        if (textureType == TextureType::TEXTURE_1D)
            return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
        else if (textureType == TextureType::TEXTURE_2D)
            return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        else if (textureType == TextureType::TEXTURE_3D)
            return D3D12_RESOURCE_DIMENSION_TEXTURE3D;

        return D3D12_RESOURCE_DIMENSION_UNKNOWN;
    }

    D3D12_RESOURCE_DESC GetResourceDesc(
        const TextureDesc& textureDesc)
    {
        uint16_t blockWidth = static_cast<uint16_t>(GetFormatProps(textureDesc.format).blockWidth);

        return D3D12_RESOURCE_DESC{
            .Dimension        = GetResourceDimension(textureDesc.type),
            .Alignment        = textureDesc.sampleNum > 1 ? 0ull : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
            .Width            = Math::AlignUp(textureDesc.width, blockWidth),
            .Height           = Math::AlignUp(textureDesc.height, blockWidth),
            .DepthOrArraySize = textureDesc.type == TextureType::TEXTURE_3D ? textureDesc.depth : textureDesc.arraySize,
            .MipLevels        = textureDesc.mipNum,
            .Format           = DxgiFormat::Get(textureDesc.format).Typeless,
            .SampleDesc       = { textureDesc.sampleNum, 0 },
            .Layout           = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            .Flags            = GetTextureFlags(textureDesc.usageMask)
        };
    }
} // namespace Ame::Rhi::D3D12
#endif
