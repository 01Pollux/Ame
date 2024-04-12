#pragma once

#include <set>

#include <Rhi/Device/MemoryDesc.hpp>
#include <Allocator/Utils/Buddy.hpp>

#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>

namespace Ame::Rhi
{
    class MemoryAllocator
    {
    public:
        MemoryAllocator(
            const MemoryAllocatorDesc& Desc);

        /// <summary>
        /// Create a managed buffer fully initialized with memory.
        /// </summary>
        [[nodiscard]] Buffer CreateBuffer(
            DeviceImpl&            RhiDevice,
            nri::MemoryLocation    Location,
            const nri::BufferDesc& Desc);

        /// <summary>
        /// Create a managed texture fully initialized with memory.
        /// </summary>
        [[nodiscard]] Texture CreateTexture(
            DeviceImpl&             RhiDevice,
            nri::MemoryLocation     Location,
            const nri::TextureDesc& Desc);

    public:
        /// <summary>
        /// Release the memory associated with the resource.
        /// </summary>
        void Release(
            DeviceImpl& RhiDevice,
            void*       Resource);

    private:
        /// <summary>
        /// Allocate dedicated memory for a resource.
        /// </summary>
        [[nodiscard]] nri::Memory* AllocateDedicatedMemory(
            DeviceImpl&            RhiDevice,
            void*                  Resource,
            const nri::MemoryDesc& Desc);

        /// <summary>
        /// Bind the memory to a resource.
        /// </summary>
        static void BindBufferToMemory(
            DeviceImpl&  RhiDevice,
            nri::Memory* Memory,
            nri::Buffer& Buffer,
            size_t       Offset);

        /// <summary>
        /// Bind the memory to a resource.
        /// </summary>
        static void BindTextureToMemory(
            DeviceImpl&   RhiDevice,
            nri::Memory*  Memory,
            nri::Texture& Texture,
            size_t        Offset);

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

        struct Block : public NonCopyable
        {
            DeviceImpl*      RhiDevice = nullptr;
            nri::Memory*     Memory    = nullptr;
            Allocator::Buddy Allocator;
            size_t           SizeLeft = 0;

            Block(
                DeviceImpl&     RhiDevice,
                size_t          Size,
                nri::MemoryType Type);

            Block(Block&& Other) noexcept;
            Block& operator=(Block&& Other) noexcept;

            ~Block();

            /// <summary>
            /// Bind the memory block to a buffer.
            /// </summary>
            [[nodiscard]] ManagedHandleType Bind(
                nri::Buffer&           Buffer,
                const nri::MemoryDesc& Desc);

            /// <summary>
            /// Bind the memory block to a texture.
            /// </summary>
            [[nodiscard]] ManagedHandleType Bind(
                nri::Texture&          Texture,
                const nri::MemoryDesc& Desc);

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
                MemoryAllocator* Allocator,
                nri::MemoryType  Location);
        };

        using SegmentType = std::map<nri::MemoryType, SegmentRegion>;

        struct SegmentNode
        {
            SegmentType Segments;

        public:
            /// <summary>
            /// Bind the memory block to a resource
            /// </summary>
            template<typename ResourceTy>
            [[nodiscard]] ManagedHandleType Bind(
                MemoryAllocator*       Allocator,
                ResourceTy&            Resource,
                const nri::MemoryDesc& Desc)
            {
                auto& Region = Segments[Desc.type];

                uint32_t Attempts = 0;
                while (Attempts++ < m_MaxGrowAttempts)
                {
                    for (auto& Block : Region.Blocks)
                    {
                        if (auto Handle = Block.Bind(Resource, Desc))
                        {
                            return Handle;
                        }
                    }
                    Region.Grow(Allocator, Desc.type);
                }

                return {};
            }
        };

        /// <summary>
        /// Key is the pointer to the resource.
        /// </summary>
        using DedicatedSegments = std::unordered_map<void*, nri::Memory*>;

        /// <summary>
        /// Key is the pointer to the resource.
        /// </summary>
        using BlockHandleMap = std::map<void*, ManagedHandleType>;

    private:
        size_t   m_BlockSize;
        size_t   m_GrowSize;
        size_t   m_MaxSize;
        float    m_GrowthFactor;
        uint32_t m_MaxGrowAttempts;

        SegmentNode       m_Node;
        DedicatedSegments m_DedicatedSegments;
        BlockHandleMap    m_BlockHandles;
    };
} // namespace Ame::Rhi