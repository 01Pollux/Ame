#pragma once

#include <Rhi/Device/Wrapper/VK/Core.hpp>
#include <Rhi/Device/Wrapper/DeviceMemoryAllocator.hpp>

#include <vk_mem_alloc.h>

#include <Rhi/Device/MemoryDesc.hpp>

namespace Ame::Rhi::VK
{
    [[nodiscard]] VkBufferCreateInfo GetBufferCreateInfo(
        const BufferDesc&         bufferDesc,
        std::span<const uint32_t> queueFamilyIndices,
        bool                      hasAddressMode);

    [[nodiscard]] static constexpr VkImageAspectFlags GetImageAspectFlags(
        ResourceFormat format)
    {
        switch (format)
        {
        case ResourceFormat::D16_UNORM:
        case ResourceFormat::D32_SFLOAT:
        case ResourceFormat::R24_UNORM_X8:
        case ResourceFormat::R32_SFLOAT_X8_X24:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case ResourceFormat::D24_UNORM_S8_UINT:
        case ResourceFormat::D32_SFLOAT_S8_UINT_X24:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        case ResourceFormat::X32_G8_UINT_X24:
        case ResourceFormat::X24_G8_UINT:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    [[nodiscard]] VkImageCreateInfo GetTextureCreateInfo(
        const TextureDesc&        textureDesc,
        std::span<const uint32_t> queueFamilyIndices,
        bool                      hasProgrammableSampleLocation);
} // namespace Ame::Rhi::VK
