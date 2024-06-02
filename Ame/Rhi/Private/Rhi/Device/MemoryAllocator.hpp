#pragma once

#include <set>
#include <mutex>

#include <Rhi/Device/MemoryDesc.hpp>
#include <Allocator/Utils/Buddy.hpp>

#include <Rhi/Descs/Resource.hpp>
#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    class MemoryAllocator
    {
        enum class AllocationType : uint8_t
        {
            Buffer,
            Texture,
        };

    public:
        /// <summary>
        /// Initialize the memory allocator.
        /// </summary>
        void Initialize(
            DeviceImpl&                rhiDevice,
            const MemoryAllocatorDesc& memoryAllocatorDesc);

        /// <summary>
        /// Release all memory associated with the allocator.
        /// </summary>
        void Shutdown();

    public:
        /// <summary>
        /// Create a managed buffer fully initialized with memory.
        /// </summary>
        [[nodiscard]] nri::Buffer* CreateBuffer(
            MemoryLocation    location,
            const BufferDesc& desc);

        /// <summary>
        /// Create a managed texture fully initialized with memory.
        /// </summary>
        [[nodiscard]] nri::Texture* CreateTexture(
            MemoryLocation     location,
            const TextureDesc& desc);

    public:
        /// <summary>
        /// Release the memory associated with the resource.
        /// resource must be a pointer to the nri texture or nri buffer.
        /// </summary>
        void Release(
            void* resource);

    private:
        /// <summary>
        /// Release the memory associated with the resource.
        /// </summary>
        void ReleaseOfType(
            void*          resource,
            AllocationType type);

    private:
        /// <summary>
        /// Allocate dedicated memory for a resource.
        /// </summary>
        [[nodiscard]] nri::Memory* AllocateDedicatedMemory(
            void*                  resource,
            const nri::MemoryDesc& desc);

        /// <summary>
        /// Bind the memory to a resource.
        /// </summary>
        static void BindBufferToMemory(
            DeviceImpl&  rhiDevice,
            nri::Memory* memory,
            nri::Buffer& buffer,
            size_t       offset);

        /// <summary>
        /// Bind the memory to a resource.
        /// </summary>
        static void BindTextureToMemory(
            DeviceImpl&   rhiDevice,
            nri::Memory*  memory,
            nri::Texture& texture,
            size_t        offset);

    private:
        struct ManagedHandleType
        {
            Allocator::Buddy*        Allocator = nullptr;
            Allocator::Buddy::Handle AllocHandle;

            [[nodiscard]] operator bool() const
            {
                return Allocator != nullptr;
            }
        };

        struct Block
        {
            DeviceImpl*      RhiDevice = nullptr;
            nri::Memory*     Memory    = nullptr;
            Allocator::Buddy Allocator;
            size_t           SizeLeft = 0;

            Block(
                DeviceImpl&     rhiDevice,
                size_t          size,
                nri::MemoryType type);

            Block(const Block&) = delete;
            Block(Block&& other) noexcept;

            Block& operator=(const Block&) = delete;
            Block& operator=(Block&& other) noexcept;

            ~Block();

            /// <summary>
            /// Bind the memory block to a buffer.
            /// </summary>
            [[nodiscard]] ManagedHandleType Bind(
                nri::Buffer&           buffer,
                const nri::MemoryDesc& desc);

            /// <summary>
            /// Bind the memory block to a texture.
            /// </summary>
            [[nodiscard]] ManagedHandleType Bind(
                nri::Texture&          texture,
                const nri::MemoryDesc& desc);

            auto operator<=>(const Block& Other) const
            {
                return SizeLeft <=> Other.SizeLeft;
            }

        private:
            void Release();
        };

        /// <summary>
        /// A segment region is a collection of blocks for a single node.
        /// Segments are sorted by size left. for best fit allocation.
        /// </summary>
        struct SegmentRegion
        {
            std::vector<Block> Blocks;

            /// <summary>
            /// Grow the segment for the given node.
            /// </summary>
            void Grow(
                DeviceImpl&      rhiDevice,
                MemoryAllocator* allocator,
                nri::MemoryType  location);
        };

        using SegmentType = std::map<nri::MemoryType, SegmentRegion>;

        /// <summary>
        /// A segment node is a collection of segment regions by memory type for a single node.
        /// </summary>
        struct SegmentNode
        {
        public:
            /// <summary>
            /// Bind the memory block to a resource
            /// </summary>
            template<typename ResourceTy>
            [[nodiscard]] ManagedHandleType Bind(
                MemoryAllocator*       allocator,
                ResourceTy&            resource,
                const nri::MemoryDesc& desc)
            {
                auto& region = m_Segments[desc.type];

                uint32_t attempts = 0;
                while (attempts++ <= allocator->m_MaxGrowAttempts)
                {
                    for (auto& block : region.Blocks)
                    {
                        if (auto handle = block.Bind(resource, desc))
                        {
                            return handle;
                        }
                    }
                    region.Grow(*allocator->m_RhiDevice, allocator, desc.type);
                }

                return {};
            }

            /// <summary>
            /// Release all memory associated with the node.
            /// </summary>
            void Release()
            {
                m_Segments.clear();
            }

        private:
            SegmentType m_Segments;
        };

        struct DedicatedAllocation
        {
            nri::Memory*   Memory;
            AllocationType Type;
        };

        struct BlockAllocation : ManagedHandleType
        {
            AllocationType Type;
        };

        /// <summary>
        /// Key is the pointer to the resource.
        /// </summary>
        using DedicatedSegments = std::unordered_map<void*, DedicatedAllocation>;

        /// <summary>
        /// Key is the pointer to the resource.
        /// </summary>
        using BlockHandleMap = std::map<void*, BlockAllocation>;

    private:
        size_t   m_BlockSize       = 0;
        size_t   m_GrowSize        = 0;
        size_t   m_MaxSize         = 0;
        float    m_GrowthFactor    = 0.f;
        uint32_t m_MaxGrowAttempts = 0;

        DeviceImpl* m_RhiDevice = nullptr;

        SegmentNode       m_Node;
        DedicatedSegments m_DedicatedSegments;
        BlockHandleMap    m_BlockHandles;

        std::mutex m_AllocationMutex;
    };
} // namespace Ame::Rhi