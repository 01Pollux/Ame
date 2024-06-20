#include <Rhi/Device/Wrapper/VK/Resource.hpp>

namespace Ame::Rhi::VK
{
    [[nodiscard]] static constexpr VkBufferUsageFlags GetBufferUsageFlags(
        BufferUsageBits bufferUsageMask,
        uint32_t        structureStride)
    {
        VkBufferUsageFlags flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        if (bufferUsageMask & BufferUsageBits::VERTEX_BUFFER)
        {
            flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }

        if (bufferUsageMask & BufferUsageBits::INDEX_BUFFER)
        {
            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        if (bufferUsageMask & BufferUsageBits::CONSTANT_BUFFER)
        {
            flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }

        if (bufferUsageMask & BufferUsageBits::ARGUMENT_BUFFER)
        {
            flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        if (bufferUsageMask & BufferUsageBits::RAY_TRACING_BUFFER)
        {
            // TODO: add more usage bits?
            flags |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR |
                     VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR |
                     VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        if (bufferUsageMask & BufferUsageBits::ACCELERATION_STRUCTURE_BUILD_READ)
        {
            flags |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
        }

        if (bufferUsageMask & BufferUsageBits::SHADER_RESOURCE)
        {
            bool isTextureStorage = structureStride == 0;
            flags |= (isTextureStorage ? VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        }

        if (bufferUsageMask & BufferUsageBits::SHADER_RESOURCE_STORAGE)
        {
            bool hasRayTracing = structureStride == 0 && (bufferUsageMask & BufferUsageBits::RAY_TRACING_BUFFER) == 0;
            flags |= (hasRayTracing ? VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT : VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
        }

        return flags;
    }

    //

    VkBufferCreateInfo GetBufferCreateInfo(
        const BufferDesc&         bufferDesc,
        std::span<const uint32_t> queueFamilyIndices,
        bool                      hasAddressMode)
    {
        auto flags = GetBufferUsageFlags(bufferDesc.usageMask, bufferDesc.structureStride);
        if (hasAddressMode)
        {
            flags |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
        }

        return VkBufferCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size                  = bufferDesc.size,
            .usage                 = flags,
            .sharingMode           = (queueFamilyIndices.size() > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size()),
            .pQueueFamilyIndices   = queueFamilyIndices.data()
        };
    }

    //

    [[nodiscard]] static constexpr VkImageUsageFlags GetImageUsageFlags(
        TextureUsageBits textureUsageBits)
    {
        VkImageUsageFlags flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (textureUsageBits & TextureUsageBits::SHADER_RESOURCE)
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;

        if (textureUsageBits & TextureUsageBits::SHADER_RESOURCE_STORAGE)
            flags |= VK_IMAGE_USAGE_STORAGE_BIT;

        if (textureUsageBits & TextureUsageBits::COLOR_ATTACHMENT)
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (textureUsageBits & TextureUsageBits::DEPTH_STENCIL_ATTACHMENT)
            flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        return flags;
    }

    static VkImageType GetImageType(
        TextureType textureType)
    {
        switch (textureType)
        {
        case nri::TextureType::TEXTURE_1D:
            return VK_IMAGE_TYPE_1D;
        case nri::TextureType::TEXTURE_2D:
            return VK_IMAGE_TYPE_2D;
        case nri::TextureType::TEXTURE_3D:
            return VK_IMAGE_TYPE_3D;
        default:
            std::unreachable();
        }
    }

    static VkImageCreateFlags GetImageCreateFlags(
        const TextureDesc& textureDesc,
        bool               programmableSampleLocationsTier)
    {
        VkImageCreateFlags flags = VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;

        const auto& formatProps = GetFormatProps(textureDesc.format);
        if (formatProps.blockWidth > 1)
        {
            // format can be used to create a view with an uncompressed format (1 texel covers 1 block)
            flags |= VK_IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE_BIT;
        }
        if (textureDesc.arraySize >= 6 && textureDesc.width == textureDesc.height)
        {
            // allow cube maps
            flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        }
        if (textureDesc.type == nri::TextureType::TEXTURE_3D)
        {
            // allow 3D demotion to a set of layers
            // TODO: hook up "VK_EXT_image_2d_view_of_3d"?
            flags |= VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT;
        }
        if (textureDesc.format >= ResourceFormat::D16_UNORM && programmableSampleLocationsTier)
        {
            flags |= VK_IMAGE_CREATE_SAMPLE_LOCATIONS_COMPATIBLE_DEPTH_BIT_EXT;
        }

        return flags;
    }

    VkImageCreateInfo GetTextureCreateInfo(
        const TextureDesc&        textureDesc,
        std::span<const uint32_t> queueFamilyIndices,
        bool                      hasProgrammableSampleLocation)
    {
        return VkImageCreateInfo{
            .sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags                 = GetImageCreateFlags(textureDesc, hasProgrammableSampleLocation),
            .imageType             = GetImageType(textureDesc.type),
            .format                = static_cast<VkFormat>(nri::nriConvertNRIFormatToVK(textureDesc.format)),
            .extent                = { textureDesc.width, textureDesc.height, textureDesc.depth },
            .mipLevels             = textureDesc.mipNum,
            .arrayLayers           = textureDesc.arraySize,
            .samples               = static_cast<VkSampleCountFlagBits>(textureDesc.sampleNum),
            .tiling                = VK_IMAGE_TILING_OPTIMAL,
            .usage                 = GetImageUsageFlags(textureDesc.usageMask),
            .sharingMode           = (queueFamilyIndices.size() > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size()),
            .pQueueFamilyIndices   = queueFamilyIndices.data(),
            .initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED
        };
    }
} // namespace Ame::Rhi::VK