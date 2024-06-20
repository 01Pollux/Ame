#pragma once

#include <Rhi/Device/Wrapper/VK/Resource.hpp>
#include <mutex>

namespace Ame::Rhi::VK
{
    struct VkDeviceMemoryAllocatorDesc
    {
        Ref<nri::Device>              NriDevice;
        CRef<nri::CoreInterface>      NriCore;
        CRef<nri::WrapperVKInterface> NriVk;

        CRef<MemoryAllocatorDesc>   MemoryDesc;
        CRef<VkAllocationCallbacks> AllocationCallbacks;

        PFN_vkGetInstanceProcAddr GetInstanceProcAddrVk;
        PFN_vkGetDeviceProcAddr   GetDeviceProcAddrVk;

        VkInstance       Instance;
        VkPhysicalDevice PhysicalDevice;
        VkDevice         LogicalDevice;

        std::span<const uint32_t> QueueFamilies;
        uint32_t                  VulkanVersion;
        bool                      IsDeviceAddressSupported;
    };

    class VKDeviceMemoryAllocator : public IDeviceMemoryAllocator
    {
    private:
        struct AllocationMetadata
        {
            VmaAllocation Allocation = nullptr;
            nri::Memory*  Memory     = nullptr;
            bool          ShouldMap  = false;
        };

        using MetadataMap = std::unordered_map<const void*, AllocationMetadata>;

    public:
        VKDeviceMemoryAllocator(
            const VkDeviceMemoryAllocatorDesc& desc);

        ~VKDeviceMemoryAllocator();

    public:
        [[nodiscard]] nri::Buffer* CreateBuffer(
            MemoryLocation    memoryLocation,
            const BufferDesc& bufferDesc) override;

        void ReleaseBuffer(
            nri::Buffer& buffer) override;

        [[nodiscard]] nri::Texture* CreateTexture(
            const TextureDesc& textureDesc) override;

        void ReleaseTexture(
            nri::Texture& texture) override;

    private:
        /// <summary>
        /// Allocate memory for the given allocation.
        /// </summary>
        [[nodiscard]] nri::Memory* AllocateMemory(
            const VmaAllocationInfo& allocationInfo) const;

    private:
        /// <summary>
        /// Create an nri buffer and bind it to the memory.
        /// </summary>
        [[nodiscard]] nri::Buffer* CreateAndBindToMemory(
            const VmaAllocationInfo& allocationInfo,
            const BufferDesc&        bufferDesc,
            VkBuffer                 buffer,
            nri::Memory*             nriMemory);

        /// <summary>
        /// Create an nri texture and bind it to the memory.
        /// </summary>
        [[nodiscard]] nri::Texture* CreateAndBindToMemory(
            const VkImageCreateInfo& textureCreateInfo,
            const VmaAllocationInfo& allocationInfo,
            const TextureDesc&       textureDesc,
            VkImage                  texture,
            nri::Memory*             nriMemory);

    private:
        /// <summary>
        /// Save allocation of the resource to keep it alive
        /// </summary>
        void SaveAllocation(
            void*                     resource,
            const AllocationMetadata& metadata);

        /// <summary>
        /// Prepare resource to be released once the scope end
        /// </summary>
        [[nodiscard]] AllocationMetadata ReleaseAllocation(
            void* resource);

    private:
        Ref<nri::Device>              m_NriDevice;
        CRef<nri::CoreInterface>      m_NriCore;
        CRef<nri::WrapperVKInterface> m_NriVk;

        VmaAllocator m_Allocator;

        std::mutex  m_MetadataLock;
        MetadataMap m_Metadatas;

        std::span<const uint32_t> m_QueueFamilies;
        bool                      m_IsDeviceAddressSupported;
    };
} // namespace Ame::Rhi::VK
