#pragma once

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
        struct Handle
        {
            static constexpr uint32_t InvalidValue = std::numeric_limits<uint32_t>::max();

            uint32_t BlockSlot = InvalidValue;
            uint32_t Offset    = InvalidValue;
            uint32_t Size      = InvalidValue;

            operator bool() const
            {
                return BlockSlot != InvalidValue;
            }
        };

        static constexpr Handle InvalidHandle = {};

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
            const uint32_t slot) const
        {
            return m_Blocks[slot].BufferRef;
        }

    public:
        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            const Handle& handle,
            const void*   data,
            size_t        size)
        {
            Write(handle.BlockSlot, handle.Offset, data, size);
        }

        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            uint32_t    slot,
            uint32_t    offset,
            const void* data,
            size_t      size)
        {
            auto& Block = m_Blocks[slot];
            Block.Stream->seekp(offset);
            Block.Stream->write(std::bit_cast<const char*>(data), size);
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
            const void* data,
            size_t      size)
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
            const Handle& slot)
        {
            m_Blocks[slot.BlockSlot].Buddy.Free({ .Offset = slot.Offset,
                                                  .Size   = slot.Size });
        }

    public:
        void Reset()
        {
            for (auto& block : m_Blocks)
            {
                block.Buddy = Allocator::Buddy(m_Desc.Size);
                block.Stream->seekp(0);
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
                    handle = { .BlockSlot = i,
                               .Offset    = static_cast<uint32_t>(foundHandle.Offset),
                               .Size      = static_cast<uint32_t>(foundHandle.Size) };
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
            if (m_Desc.Size < size ||
                (m_Desc.MaxBlockCount && m_Desc.MaxBlockCount <= m_Blocks.size())) [[unlikely]]
            {
                return {};
            }

            m_Blocks.emplace_back(m_Device.get(), m_Desc.Size, m_Desc.UsageFlags);
            return { .BlockSlot = static_cast<uint32_t>(m_Blocks.size() - 1),
                     .Offset    = 0,
                     .Size      = static_cast<uint32_t>(size) };
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
        Ref<Device>          m_Device;
        BlockBasedBufferDesc m_Desc;

        struct Block
        {
            Allocator::Buddy               Buddy;
            Buffer                         BufferRef;
            UPtr<Streaming::BufferOStream> Stream;

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
    };
} // namespace Ame::Rhi::Util