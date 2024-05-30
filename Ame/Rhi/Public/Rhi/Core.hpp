#pragma once

#include <span>

#include <Core/Ame.hpp>
#include <NRI/NRI.h>
#include <NRI/Extensions/NRIHelper.h>

namespace Ame::Rhi
{
    class Device;
    class DeviceImpl;
    class CommandList;
    class CommandListImpl;
    class PipelineLayout;
    class PipelineState;
    class Buffer;
    class Texture;
    class ResourceView;
    class DescriptorSet;

    using Mip_t    = uint8_t;
    using Dim_t    = uint16_t;
    using Sample_t = uint8_t;

    using DeviceDesc     = nri::DeviceDesc;
    using ResourceFormat = nri::Format;
    using FormatProps    = nri::FormatProps;

    enum class GraphicsAPI : uint8_t
    {
        Null,
        DirectX12,
        Vulkan,

        Count,
    };

    enum class DeviceFeatureType : uint8_t
    {
        Disabled,
        Optional,
        Required
    };

    // Color space:
    //  - BT.709 - LDR https://en.wikipedia.org/wiki/Rec._709
    //  - BT.2020 - HDR https://en.wikipedia.org/wiki/Rec._2020
    // Transfer function:
    //  - G10 - linear (gamma 1.0)
    //  - G22 - sRGB (gamma ~2.2)
    //  - G2084 - SMPTE ST.2084 (Perceptual Quantization)
    // Bits per channel:
    //  - 8, 10, 16 (float)
    enum class SwapChainFormat : uint8_t
    {
        BT709_G10_16BIT,
        BT709_G22_8BIT,
        BT709_G22_10BIT,
        BT2020_G2084_10BIT,

        Count,
    };

    //

    template<typename Ty>
    [[nodiscard]] constexpr size_t Count64(
        const Ty& value)
    {
        return std::size(value);
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr size_t Size64()
    {
        return sizeof(Ty);
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr size_t Size64(
        const Ty&)
    {
        return sizeof(Ty);
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint32_t Count32(
        const Ty& value)
    {
        return static_cast<uint32_t>(std::size(value));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint32_t Size32()
    {
        return static_cast<uint32_t>(sizeof(Ty));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint32_t Size32(
        const Ty&)
    {
        return static_cast<uint32_t>(sizeof(Ty));
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint16_t Count16(
        const Ty& value)
    {
        return static_cast<uint16_t>(std::size(value));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint16_t Size16()
    {
        return static_cast<uint16_t>(sizeof(Ty));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint16_t Size16(
        const Ty&)
    {
        return static_cast<uint16_t>(sizeof(Ty));
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint8_t Count8(
        const Ty& value)
    {
        return static_cast<uint8_t>(std::size(value));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint8_t Size8()
    {
        return static_cast<uint8_t>(sizeof(Ty));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint8_t Size8(
        const Ty&)
    {
        return static_cast<uint8_t>(sizeof(Ty));
    }

    /// <summary>
    /// Get format properties for the specified format.
    /// </summary>
    const FormatProps& GetFormatProps(
        ResourceFormat format);
} // namespace Ame::Rhi