#pragma once

#include <boost/container/flat_set.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Stream/Buffer.hpp>
#include <Allocator/Utils/Buddy.hpp>

namespace Ame::Rhi::Util
{
    struct BlockBasedBufferDesc
    {
        /// <summary>
        /// Initial size of the buffer
        /// </summary>
        uint32_t Size = 0xFFFFFF;

        /// <summary>
        /// Maximum number of blocks in the buffer
        /// </summary>
        uint32_t MaxBlockCount = 0;

        /// <summary>
        /// The usage flags of the buffer
        /// </summary>
        BufferUsageBits UsageFlags = BufferUsageBits::NONE;
    };

    /// <summary>
    /// Buffers that are based on blocks
    /// Each block is a buffer that can be written to
    /// When a block is full, a new block is created until the maximum number of blocks is reached
    /// </summary>
    class BlockBasedBuffer
    {
    public:
        static constexpr uint32_t c_InvalidValue   = std::numeric_limits<uint32_t>::max();
        static constexpr uint64_t c_InvalidValue64 = std::numeric_limits<uint64_t>::max();

        struct Handle
        {
            uint32_t BlockSlot = c_InvalidValue;
            uint32_t Offset    = c_InvalidValue;
            uint32_t Size      = c_InvalidValue;

            operator bool() const
            {
                return BlockSlot != c_InvalidValue;
            }
        };

        static constexpr Handle c_InvalidHandle = {};

    private:
        using BusyHandle = Allocator::Buddy::Handle;

        struct RetiredHandle : Handle
        {
            uint64_t FrameIndex = c_InvalidValue64;

            [[nodiscard]] auto operator<=>(
                const RetiredHandle& other) const
            {
                return FrameIndex <=> other.FrameIndex;
            }
        };

        using BusyHandleSet    = boost::container::flat_set<BusyHandle>;
        using RetiredHandleSet = boost::container::flat_multiset<RetiredHandle>;
        using BufferStream     = UPtr<Streaming::BufferOStream>;

    public:
        BlockBasedBuffer(
            Device&                     rhiDevice,
            const BlockBasedBufferDesc& desc = {}) :
            m_Device(rhiDevice),
            m_Desc(desc)
        {
        }

    public:
        /// <summary>
        /// Flush the stream to the buffer
        /// </summary>
        void Flush(
            const uint32_t blockSlot)
        {
            AllocationMustExists(blockSlot);
            m_Blocks[blockSlot].Stream->flush();
        }

        /// <summary>
        /// Flush all streams to the buffer
        /// </summary>
        void FlushAll()
        {
            for (auto& block : m_Blocks)
            {
                block.Stream->flush();
            }
        }

    public:
        /// <summary>
        /// Get the size of the block
        /// </summary>
        [[nodiscard]] uint32_t GetBlockSize() const
        {
            return m_Desc.Size;
        }

        /// <summary>
        /// Get the number of blocks in the buffer
        /// </summary>
        [[nodiscard]] uint32_t GetBlocksCount() const
        {
            return static_cast<uint32_t>(m_Blocks.size());
        }

    public:
        /// <summary>
        /// Get buffer of the slot based buffer
        /// </summary>
        [[nodiscard]] const Buffer& GetBuffer(
            uint32_t blockSlot) const
        {
            AllocationMustExists(blockSlot);
            return m_Blocks[blockSlot].BufferRef;
        }

        /// <summary>
        /// Get buffer of the slot based buffer
        /// </summary>
        [[nodiscard]] const Buffer& GetBuffer(
            const Handle& handle) const
        {
            AllocationMustExists(handle);
            return GetBuffer(handle.BlockSlot);
        }

    public:
        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            const Handle&    handle,
            const std::byte* data,
            size_t           size)
        {
            AllocationMustExists(handle);
            Write(handle.BlockSlot, handle.Offset, data, size);
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t         slot,
            uint32_t         offset,
            const std::byte* data,
            size_t           size)
        {
            AllocationMustExists(slot);
            auto& block = m_Blocks[slot];
            block.Stream->seekp(offset);
            block.Stream->write(std::bit_cast<const char*>(data), size);
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] Handle Rent(
            size_t size)
        {
            return FindSlot(size);
        }

        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] Handle Rent(
            const std::byte* data,
            size_t           size)
        {
            auto handle = FindSlot(size);
            if (handle)
            {
                Write(handle, data, size);
            }
            return handle;
        }

        /// <summary>
        /// Return a slot in the buffer
        /// </summary>
        void Return(
            const Handle& handle)
        {
            AllocationMustExists(handle);
            RetireHandle(
                handle.BlockSlot,
                { .Offset = handle.Offset, .Size = handle.Size },
                m_Device.get().GetFrameCount());
        }

    public:
        void Reset()
        {
            uint64_t currentFrameIndex = m_Device.get().GetFrameCount();
            RetireAllHandles(currentFrameIndex);
        }

    private:
        /// <summary>
        /// Find a slot in the buffer with the given size, if not found, create a new block
        /// </summary>
        [[nodiscard]] Handle FindSlot(
            uint32_t size)
        {
            // If the size is larger than the buffer, return an invalid handle
            if (size > c_InvalidValue) [[unlikely]]
            {
                std::unreachable();
            }

            uint64_t currentFrameIndex = m_Device.get().GetFrameCount();
            Handle   handle;

            DiscardRetiredHandles(currentFrameIndex);

            for (uint32_t i = 0; i < m_Blocks.size(); i++)
            {
                auto& block       = m_Blocks[i];
                auto  foundHandle = block.Buddy.Allocate(size);

                if (foundHandle)
                {
                    handle = {
                        .BlockSlot = i,
                        .Offset    = static_cast<uint32_t>(foundHandle.Offset),
                        .Size      = static_cast<uint32_t>(foundHandle.Size)
                    };
                    break;
                }
            }

            if (!handle)
            {
                handle = CreateBlock(size);
            }

            RentHandle(handle);
            return handle;
        }

        /// <summary>
        /// Create a new block in the buffer with the given size
        /// If the size is larger than the buffer, return an invalid handle
        /// </summary>
        [[nodiscard]] Handle CreateBlock(
            uint32_t size)
        {
            if (m_Desc.Size < size ||
                (m_Desc.MaxBlockCount && m_Desc.MaxBlockCount <= m_Blocks.size())) [[unlikely]]
            {
                return {};
            }

            auto& block  = m_Blocks.emplace_back(m_Device.get(), m_Desc.Size, m_Desc.UsageFlags);
            auto  handle = block.Buddy.Allocate(size);

            return {
                .BlockSlot = static_cast<uint32_t>(m_Blocks.size() - 1),
                .Offset    = static_cast<uint32_t>(handle.Offset),
                .Size      = static_cast<uint32_t>(handle.Size)
            };
        }

    private:
        /// <summary>
        /// Rent a handle in the buffer
        /// </summary>
        void RentHandle(
            const Handle& handle)
        {
            auto& block = m_Blocks[handle.BlockSlot];
            block.BusyHandles.insert({ .Offset = handle.Offset, .Size = handle.Size });
        }

        /// <summary>
        /// Rent all handles in the buffer
        /// </summary>
        void RetireAllHandles(
            uint64_t frameIndex)
        {
            for (uint32_t i = 0; i < m_Blocks.size(); i++)
            {
                auto& block = m_Blocks[i];
                for (auto& handle : block.BusyHandles)
                {
                    m_RetiredHandles.insert(RetiredHandle{
                        { .BlockSlot = i,
                          .Offset    = static_cast<uint32_t>(handle.Offset),
                          .Size      = static_cast<uint32_t>(handle.Size) },
                        {
                            frameIndex,
                        } });
                }
                block.BusyHandles.clear();
            }
        }

        /// <summary>
        /// Retire a handle in the buffer
        /// </summary>
        void RetireHandle(
            uint32_t                        blockSlot,
            const Allocator::Buddy::Handle& handle,
            uint64_t                        frameIndex)
        {
            auto& block = m_Blocks[blockSlot];
            block.BusyHandles.erase({ handle.Offset, handle.Size });
            m_RetiredHandles.insert(RetiredHandle{
                { .BlockSlot = blockSlot,
                  .Offset    = static_cast<uint32_t>(handle.Offset),
                  .Size      = static_cast<uint32_t>(handle.Size) },
                { frameIndex } });
        }

        /// <summary>
        /// Flush all frames that are older than the given frame index + frame count
        /// </summary>
        void DiscardRetiredHandles(
            uint64_t currentFrameIndex)
        {
            uint8_t frameCount = m_Device.get().GetFrameCountInFlight();
            if (frameCount > currentFrameIndex) [[unlikely]]
            {
                return;
            }

            currentFrameIndex -= frameCount;

            auto it = m_RetiredHandles.begin();
            while (it != m_RetiredHandles.end())
            {
                if (it->FrameIndex <= currentFrameIndex)
                {
                    auto& block = m_Blocks[it->BlockSlot];
                    block.Buddy.Free(
                        { .Offset = it->Offset,
                          .Size   = it->Size });

                    it = m_RetiredHandles.erase(it);
                }
                else
                {
                    break;
                }
            }
        }

    private:
        /// <summary>
        /// Create an empty buffer
        /// </summary>
        [[nodiscard]] static Buffer CreateBuffer(
            Device&         rhiDevice,
            uint32_t        size,
            BufferUsageBits usageFlags)
        {
            BufferDesc desc{
                .size      = size,
                .usageMask = usageFlags
            };

            return Buffer(
                rhiDevice,
                MemoryLocation::HOST_UPLOAD,
                desc);
        }

    private:
        void AllocationMustExists(
            uint32_t blockSlot) const
        {
#ifdef AME_DEBUG
            if (blockSlot >= m_Blocks.size()) [[unlikely]]
            {
                std::unreachable();
            }
#endif
        }

        void AllocationMustExists(
            const Handle& handle) const
        {
#ifdef AME_DEBUG
            AllocationMustExists(handle.BlockSlot);
            auto& block = m_Blocks[handle.BlockSlot];
            if (!block.BusyHandles.contains({ .Offset = handle.Offset, .Size = handle.Size })) [[unlikely]]
            {
                std::unreachable();
            }
#endif
        }

    private:
        Ref<Device>          m_Device;
        BlockBasedBufferDesc m_Desc;

        struct Block
        {
            Allocator::Buddy Buddy;
            BusyHandleSet    BusyHandles;
            Buffer           BufferRef;
            BufferStream     Stream;

            Block(
                Device&         rhiDevice,
                uint32_t        size,
                BufferUsageBits usageFlags) :
                Buddy(size),
                BufferRef(CreateBuffer(rhiDevice, size, usageFlags)),
                Stream(std::make_unique<Streaming::BufferOStream>(Streaming::BufferView(BufferRef)))
            {
            }
        };

        std::vector<Block> m_Blocks;

        // Sorted by frame index
        RetiredHandleSet m_RetiredHandles;
    };
} // namespace Ame::Rhi::Util