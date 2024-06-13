#pragma once

#include <boost/container/flat_set.hpp>
#include <Rhi/Util/GenericBufferStream.hpp>
#include <Allocator/Utils/Buddy.hpp>

namespace Ame::Rhi::Util
{
    struct BlockBasedBufferDesc
    {
        /// <summary>
        /// Initial size of the buffer
        /// </summary>
        size_t Size = 0x7FF'FFE;

        /// <summary>
        /// Maximum number of blocks in the buffer
        /// </summary>
        uint32_t MaxBlockCount = 0;

        /// <summary>
        /// Grow factor of the buffer
        /// </summary>
        float GrowFactor = 2.0f;

        /// <summary>
        /// The usage flags of the buffer
        /// </summary>
        BufferUsageBits UsageFlags = BufferUsageBits::NONE;

        /// <summary>
        /// The memory location of the buffer
        /// </summary>
        MemoryLocation Location = MemoryLocation::HOST_UPLOAD;
    };

    /// <summary>
    /// Buffers that are based on blocks
    /// Each block is a buffer that can be written to
    /// When a block is full, a new block is created until the maximum number of blocks is reached
    /// </summary>
    template<bool WithStreaming = true>
    class BlockBasedBuffer
    {
    public:
        using DescType                             = BlockBasedBufferDesc;
        static constexpr uint32_t c_InvalidValue   = std::numeric_limits<uint32_t>::max();
        static constexpr uint64_t c_InvalidValue64 = std::numeric_limits<uint64_t>::max();

        struct Handle
        {
            uint32_t BlockSlot = c_InvalidValue;
            size_t   Offset    = c_InvalidValue64;
            size_t   Size      = c_InvalidValue64;

            explicit operator bool() const noexcept
            {
                return BlockSlot != c_InvalidValue;
            }
        };

        static constexpr Handle c_InvalidHandle = {};

    private:
        using BufferStream = GenericBufferStream<WithStreaming>;

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
            BlockSlotMustExists(blockSlot);
            m_Blocks[blockSlot].Stream->flush();
        }

        /// <summary>
        /// Flush all streams to the buffer
        /// </summary>
        void FlushAll()
        {
            for (auto& block : m_Blocks)
            {
                block.BufferRef.Flush();
            }
        }

    public:
        /// <summary>
        /// Get the size of the block
        /// </summary>
        [[nodiscard]] size_t GetBlockSize(
            uint32_t blockSlot) const
        {
            BlockSlotMustExists(blockSlot);
            return m_Blocks[blockSlot].BufferRef.GetSize();
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
            BlockSlotMustExists(blockSlot);
            return m_Blocks[blockSlot].BufferRef.GetBuffer();
        }

        /// <summary>
        /// Get buffer of the slot based buffer
        /// </summary>
        [[nodiscard]] const Buffer& GetBuffer(
            const Handle& handle) const
        {
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
            Write(handle.BlockSlot, handle.Offset, data, size);
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            size_t           slot,
            size_t           offset,
            const std::byte* data,
            size_t           size)
        {
            BlockSlotMustExists(slot);
            auto& block = m_Blocks[slot];
            block.BufferRef.Write(offset, data, size);
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
            BlockSlotMustExists(handle.BlockSlot);

            auto& block = m_Blocks[handle.BlockSlot];
            block.Buddy.Free(
                { .Offset = handle.Offset,
                  .Size   = handle.Size });
        }

    public:
        /// <summary>
        /// Reset all blocks in the buffer
        /// </summary>
        void Reset()
        {
            for (auto& block : m_Blocks)
            {
                block.Reset();
            }
        }

    private:
        /// <summary>
        /// Find a slot in the buffer with the given size, if not found, create a new block
        /// </summary>
        [[nodiscard]] Handle FindSlot(
            size_t size)
        {
            Handle handle;
            for (uint32_t i = 0; i < m_Blocks.size(); i++)
            {
                auto& block       = m_Blocks[i];
                auto  foundHandle = block.Buddy.Allocate(size);

                if (foundHandle)
                {
                    handle = {
                        .BlockSlot = i,
                        .Offset    = foundHandle.Offset,
                        .Size      = foundHandle.Size
                    };
                    break;
                }
            }

            if (!handle)
            {
                handle = CreateBlock(size);
            }

            return handle;
        }

        /// <summary>
        /// Create a new block in the buffer with the given size
        /// If the size is larger than the buffer, return an invalid handle
        /// </summary>
        [[nodiscard]] Handle CreateBlock(
            size_t size)
        {
            if (m_Desc.MaxBlockCount && m_Desc.MaxBlockCount <= m_Blocks.size()) [[unlikely]]
            {
                return {};
            }

            m_Desc.Size = GetSuitableBlockSize(size);

            auto& block  = m_Blocks.emplace_back(m_Device.get(), m_Desc.Location, m_Desc.Size, m_Desc.UsageFlags);
            auto  handle = block.Buddy.Allocate(size);

            return {
                .BlockSlot = static_cast<uint32_t>(m_Blocks.size() - 1),
                .Offset    = handle.Offset,
                .Size      = handle.Size
            };
        }

        /// <summary>
        /// Get the suitable block size for the buffer
        /// </summary>
        [[nodiscard]] uint32_t GetSuitableBlockSize(
            size_t size)
        {
            size_t blockSize = m_Desc.Size;
            if (blockSize < size)
            {
                size_t factor = static_cast<size_t>(std::ceil(static_cast<double>(size) / blockSize));
                blockSize *= factor;
            }
            return blockSize;
        }

    private:
        /// <summary>
        /// Create an empty buffer
        /// </summary>
        [[nodiscard]] static Buffer CreateBuffer(
            Device&         rhiDevice,
            MemoryLocation  location,
            size_t          size,
            BufferUsageBits usageFlags)
        {
            BufferDesc desc{
                .size      = size,
                .usageMask = usageFlags
            };

            return Buffer(
                rhiDevice,
                location,
                desc);
        }

    private:
        void BlockSlotMustExists(
            uint32_t blockSlot) const
        {
#ifdef AME_DEBUG
            if (blockSlot >= m_Blocks.size()) [[unlikely]]
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
            BufferStream     BufferRef;

            Block(
                Device&         rhiDevice,
                MemoryLocation  location,
                size_t          size,
                BufferUsageBits usageFlags) :
                Buddy(size),
                BufferRef(CreateBuffer(rhiDevice, location, size, usageFlags))
            {
            }

            void Reset()
            {
                Buddy = Allocator::Buddy(BufferRef.GetSize());
            }
        };

        std::vector<Block> m_Blocks;
    };
} // namespace Ame::Rhi::Util