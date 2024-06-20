#include <Rhi/Device/Wrapper/VK/MemoryAllocator.hpp>

#include <Core/Enum.hpp>
#include <Rhi/NriError.hpp>

namespace Ame::Rhi::VK
{
    VKDeviceMemoryAllocator::VKDeviceMemoryAllocator(
        const VkDeviceMemoryAllocatorDesc& desc) :
        m_NriDevice(desc.NriDevice),
        m_NriCore(desc.NriCore),
        m_NriVk(desc.NriVk),
        m_QueueFamilies(desc.QueueFamilies),
        m_IsDeviceAddressSupported(desc.IsDeviceAddressSupported)
    {
        using namespace EnumBitOperators;

        VmaAllocatorCreateFlags flags{};
        if (!desc.MemoryDesc.get().MultiThreaded)
        {
            flags |= VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT;
        }

        if (desc.IsDeviceAddressSupported)
        {
            flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        }

        VmaVulkanFunctions vkFunctions{
            .vkGetInstanceProcAddr = desc.GetInstanceProcAddrVk,
            .vkGetDeviceProcAddr   = desc.GetDeviceProcAddrVk
        };

        VmaAllocatorCreateInfo allocatorDesc{
            .flags                       = flags,
            .physicalDevice              = desc.PhysicalDevice,
            .device                      = desc.LogicalDevice,
            .preferredLargeHeapBlockSize = desc.MemoryDesc.get().BlockSize,
            .pAllocationCallbacks        = &desc.AllocationCallbacks.get(),
            .pVulkanFunctions            = &vkFunctions,
            .instance                    = desc.Instance,
            .vulkanApiVersion            = desc.VulkanVersion
        };

        ThrowIfVkFailed(
            vmaCreateAllocator(&allocatorDesc, &m_Allocator),
            "Failed to create vulkan memory allocator.");
    }

    VKDeviceMemoryAllocator::~VKDeviceMemoryAllocator()
    {
        vmaDestroyAllocator(m_Allocator);
    }

    //

    nri::Buffer* VKDeviceMemoryAllocator::CreateBuffer(
        MemoryLocation    memoryLocation,
        const BufferDesc& bufferDesc)
    {
        auto                    bufferCreateInfo = GetBufferCreateInfo(bufferDesc, m_QueueFamilies, m_IsDeviceAddressSupported);
        VmaAllocationCreateInfo allocationCreateInfo{};

        switch (memoryLocation)
        {
        case MemoryLocation::DEVICE:
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
            break;
        case MemoryLocation::DEVICE_UPLOAD:
        case MemoryLocation::HOST_UPLOAD:
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            break;
        case MemoryLocation::HOST_READBACK:
            allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_TO_CPU;
            break;
        }

        VkBuffer      buffer{};
        VmaAllocation allocation{};

        ThrowIfVkFailed(
            vmaCreateBuffer(m_Allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, nullptr),
            "Failed to create buffer");

        VmaAllocationInfo allocationInfo;
        vmaGetAllocationInfo(m_Allocator, allocation, &allocationInfo);

        //

        auto memory    = AllocateMemory(allocationInfo);
        auto nriBuffer = CreateAndBindToMemory(allocationInfo, bufferDesc, buffer, memory);
        SaveAllocation(buffer, { allocation, memory });

        return nriBuffer;
    }

    void VKDeviceMemoryAllocator::ReleaseBuffer(
        nri::Buffer& buffer)
    {
        auto vkBuffer = std::bit_cast<VkBuffer>(m_NriCore.get().GetBufferNativeObject(buffer));
        AME_LOG_ASSERT(Log::Rhi(), vkBuffer != nullptr, "Buffer resource is null.");

        auto [allocation, memory] = ReleaseAllocation(vkBuffer);

        m_NriCore.get().DestroyBuffer(buffer);
        m_NriCore.get().FreeMemory(*memory);
        vmaDestroyBuffer(m_Allocator, vkBuffer, allocation);
    }

    //

    nri::Texture* VKDeviceMemoryAllocator::CreateTexture(
        const TextureDesc& textureDesc)
    {
        auto                    textureCreateInfo = GetTextureCreateInfo(textureDesc, m_QueueFamilies, m_IsDeviceAddressSupported);
        VmaAllocationCreateInfo allocationCreateInfo{
            .usage = VMA_MEMORY_USAGE_GPU_ONLY
        };

        VkImage       texture{};
        VmaAllocation allocation{};

        ThrowIfVkFailed(
            vmaCreateImage(m_Allocator, &textureCreateInfo, &allocationCreateInfo, &texture, &allocation, nullptr),
            "Failed to create buffer");

        VmaAllocationInfo allocationInfo;
        vmaGetAllocationInfo(m_Allocator, allocation, &allocationInfo);

        //

        auto memory    = AllocateMemory(allocationInfo);
        auto nriBuffer = CreateAndBindToMemory(textureCreateInfo, allocationInfo, textureDesc, texture, memory);
        SaveAllocation(texture, { allocation, memory });

        return nriBuffer;
    }

    void VKDeviceMemoryAllocator::ReleaseTexture(
        nri::Texture& texture)
    {
        auto vkTexture = std::bit_cast<VkImage>(m_NriCore.get().GetTextureNativeObject(texture));
		AME_LOG_ASSERT(Log::Rhi(), vkTexture != nullptr, "Texture resource is null.");

		auto [allocation, memory] = ReleaseAllocation(vkTexture);

		m_NriCore.get().DestroyTexture(texture);
		m_NriCore.get().FreeMemory(*memory);
		vmaDestroyImage(m_Allocator, vkTexture, allocation);
    }

    //

    nri::Memory* VKDeviceMemoryAllocator::AllocateMemory(
        const VmaAllocationInfo& allocationInfo) const
    {
        nri::MemoryVKDesc memoryDesc{
            .vkDeviceMemory  = std::bit_cast<nri::NRIVkDeviceMemory>(allocationInfo.deviceMemory),
            .size            = allocationInfo.size,
            .memoryTypeIndex = allocationInfo.memoryType
        };

        nri::Memory* nriMemory;
        ThrowIfFailed(
            m_NriVk.get().CreateMemoryVK(m_NriDevice, memoryDesc, nriMemory),
            "Failed to create memory");

        return nriMemory;
    }

    //

    nri::Buffer* VKDeviceMemoryAllocator::CreateAndBindToMemory(
        const VmaAllocationInfo& allocationInfo,
        const BufferDesc&        bufferDesc,
        VkBuffer                 buffer,
        nri::Memory*             nriMemory)
    {
        nri::Buffer*      nriBuffer;
        nri::BufferVKDesc vkBufferDesc{
            .vkBuffer        = std::bit_cast<nri::NRIVkBuffer>(buffer),
            .memory          = nriMemory,
            .size            = allocationInfo.size,
            .memoryOffset    = allocationInfo.offset,
            .structureStride = bufferDesc.structureStride
        };

        ThrowIfFailed(
            m_NriVk.get().CreateBufferVK(m_NriDevice, vkBufferDesc, nriBuffer),
            "Failed to create buffer");

        nri::BufferMemoryBindingDesc bufferMemoryBindingDesc{
            .memory = nriMemory,
            .buffer = nriBuffer,
            .offset = allocationInfo.offset
        };

        ThrowIfFailed(
            m_NriCore.get().BindBufferMemory(m_NriDevice, &bufferMemoryBindingDesc, 1),
            "Failed to create buffer");

        return nriBuffer;
    }

    nri::Texture* VKDeviceMemoryAllocator::CreateAndBindToMemory(
        const VkImageCreateInfo& textureCreateInfo,
        const VmaAllocationInfo& allocationInfo,
        const TextureDesc&       textureDesc,
        VkImage                  texture,
        nri::Memory*             nriMemory)
    {
        nri::Texture*      nriTexture;
        nri::TextureVKDesc vkTextureDesc{
            .vkImage            = std::bit_cast<nri::NRIVkImage>(texture),
            .vkFormat           = static_cast<uint32_t>(textureCreateInfo.format),
            .vkImageAspectFlags = GetImageAspectFlags(textureDesc.format),
            .vkImageType        = static_cast<uint32_t>(textureCreateInfo.imageType),
            .width              = textureDesc.width,
            .height             = textureDesc.height,
            .depth              = textureDesc.depth,
            .mipNum             = textureDesc.mipNum,
            .arraySize          = textureDesc.arraySize,
            .sampleNum          = textureDesc.sampleNum
        };

        ThrowIfFailed(
            m_NriVk.get().CreateTextureVK(m_NriDevice, vkTextureDesc, nriTexture),
            "Failed to create texture");

        nri::TextureMemoryBindingDesc textureMemoryBindingDesc{
            .memory  = nriMemory,
            .texture = nriTexture,
            .offset  = allocationInfo.offset
        };

        ThrowIfFailed(
            m_NriCore.get().BindTextureMemory(m_NriDevice, &textureMemoryBindingDesc, 1),
            "Failed to create texture");

        return nriTexture;
    }

    //

    void VKDeviceMemoryAllocator::SaveAllocation(
        void*                     resource,
        const AllocationMetadata& metadata)
    {
        std::scoped_lock lock(m_MetadataLock);
        m_Metadatas[resource] = metadata;
    }

    auto VKDeviceMemoryAllocator::ReleaseAllocation(
        void* resource) -> AllocationMetadata
    {
        std::scoped_lock lock(m_MetadataLock);
        auto             iter = m_Metadatas.find(resource);

        auto metadata = iter->second;
        m_Metadatas.erase(iter);

        return metadata;
    }
} // namespace Ame::Rhi::VK
