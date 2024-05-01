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
        Rhi::BufferUsageBits UsageFlags = Rhi::BufferUsageBits::NONE;
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

            uint32_t Block  = InvalidValue;
            uint32_t Offset = InvalidValue;
            uint32_t Size   = InvalidValue;

            operator bool() const
            {
                return Block != InvalidValue;
            }
        };

    public:
        BlockBasedBuffer(
            Rhi::Device&                RhiDevice,
            const BlockBasedBufferDesc& Desc = {}) :
            m_Device(RhiDevice),
            m_Desc(Desc)
        {
        }

    public:
        /// <summary>
        /// Flush the stream to the buffer
        /// </summary>
        void Flush(
            const Handle& Slot)
        {
            m_Blocks[Slot.Block].Stream->flush();
        }

        /// <summary>
        /// Flush all streams to the buffer
        /// </summary>
        void FlushAll()
        {
            for (auto& Block : m_Blocks)
            {
                Block.Stream->flush();
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
        [[nodiscard]] const Rhi::Buffer& GetBuffer(
            const Handle& Slot) const
        {
            return m_Blocks[Slot.Block].Buffer;
        }

    public:
        /// <summary>
        /// Write data to the buffer
        /// </summary>
        void Write(
            const Handle& Slot,
            const void*   Data,
            size_t        Size)
        {
            auto& Block = m_Blocks[Slot.Block];
            Block.Stream->seekp(Slot.Offset);
            Block.Stream->write(std::bit_cast<const char*>(Data), Size);
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] Handle Rent(
            size_t Size)
        {
            return FindSlot(Size);
        }

        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] Handle Rent(
            const void* Data,
            size_t      Size)
        {
            auto Slot = FindSlot(Size);
            if (Slot)
            {
                Write(Slot, Data, Size);
            }
            return Slot;
        }

        /// <summary>
        /// Return a slot in the buffer
        /// </summary>
        void Return(
            const Handle& Slot)
        {
            m_Blocks[Slot.Block].Buddy.Free({ .Offset = Slot.Offset, .Size = Slot.Size });
        }

    private:
        /// <summary>
        /// Find a slot in the buffer with the given size, if not found, create a new block
        /// </summary>
        [[nodiscard]] Handle FindSlot(
            size_t Size)
        {
            Handle Slot;

            for (uint32_t i = 0; i < m_Blocks.size(); i++)
            {
                auto& Block     = m_Blocks[i];
                auto  FoundSlot = Block.Buddy.Allocate(Size);

                if (FoundSlot)
                {
                    Slot = { .Block = i, .Offset = static_cast<uint32_t>(FoundSlot.Offset), .Size = static_cast<uint32_t>(FoundSlot.Size) };
                    break;
                }
            }

            if (!Slot)
            {
                Slot = CreateBlock(Size);
            }
            return Slot;
        }

        /// <summary>
        /// Create a new block in the buffer with the given size
        /// If the size is larger than the buffer, return an invalid handle
        /// </summary>
        [[nodiscard]] Handle CreateBlock(
            size_t Size)
        {
            if (m_Desc.Size < Size || m_Desc.MaxBlockCount <= m_Blocks.size()) [[unlikely]]
            {
                return {};
            }

            m_Blocks.emplace_back(m_Device.get(), m_Desc.Size, m_Desc.UsageFlags);
            return { .Block = static_cast<uint32_t>(m_Blocks.size() - 1), .Offset = 0, .Size = static_cast<uint32_t>(Size) };
        }

    private:
        /// <summary>
        /// Create an empty buffer
        /// </summary>
        [[nodiscard]] static Rhi::Buffer CreateBuffer(
            Rhi::Device&         RhiDevice,
            uint32_t             Size,
            Rhi::BufferUsageBits UsageFlags)
        {
            Rhi::BufferDesc Desc{
                .size      = Size,
                .usageMask = UsageFlags
            };

            return Rhi::Buffer(
                RhiDevice,
                Rhi::MemoryLocation::HOST_UPLOAD,
                Desc);
        }

    private:
        Ref<Rhi::Device>     m_Device;
        BlockBasedBufferDesc m_Desc;

        struct Block
        {
            Allocator::Buddy                    Buddy;
            Rhi::Buffer                         Buffer;
            UPtr<Rhi::Streaming::BufferOStream> Stream;

            Block(
                Rhi::Device&         RhiDevice,
                uint32_t             Size,
                Rhi::BufferUsageBits UsageFlags) :
                Buddy(Size),
                Buffer(CreateBuffer(RhiDevice, Size, UsageFlags)),
                Stream(std::make_unique<Rhi::Streaming::BufferOStream>(Rhi::Streaming::BufferView(Buffer)))
            {
            }
        };

        std::vector<Block> m_Blocks;
    };
} // namespace Ame::Rhi::Util