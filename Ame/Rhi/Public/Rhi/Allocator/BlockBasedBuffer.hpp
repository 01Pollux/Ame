#pragma once

#include <boost/container/flat_set.hpp>
#include <Allocator/Utils/Buddy.hpp>
#include <Rhi/Device/Device.hpp>

namespace Ame::Rhi::Util
{
    struct BlockBasedBufferDesc
    {
        Dg::IRenderDevice*                Device = nullptr;
        Dg::BufferDesc                    BuffDesc;
        std::function<void(Dg::IBuffer*)> OnBufferCreate = nullptr;
        uint32_t                          MaxBlockCount  = std::numeric_limits<uint32_t>::max();
    };

    /// <summary>
    /// Buffers that are based on blocks
    /// Each block is a buffer that can be written to
    /// When a block is full, a new block is created until the maximum number of blocks is reached
    /// </summary>
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

    public:
        BlockBasedBuffer(
            const BlockBasedBufferDesc& desc = {}) :
            m_Desc(desc)
        {
        }

    public:
        /// <summary>
        /// Get the size of the block
        /// </summary>
        [[nodiscard]] size_t GetBlockSize(
            uint32_t blockSlot) const
        {
            BlockSlotMustExists(blockSlot);
            return m_Blocks[blockSlot].Buffer->GetDesc().Size;
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
        [[nodiscard]] const Dg::Ptr<Dg::IBuffer>& GetBuffer(
            uint32_t blockSlot) const
        {
            BlockSlotMustExists(blockSlot);
            return m_Blocks[blockSlot].Buffer;
        }

        /// <summary>
        /// Get buffer of the slot based buffer
        /// </summary>
        [[nodiscard]] const Dg::Ptr<Dg::IBuffer>& GetBuffer(
            const Handle& handle) const
        {
            return GetBuffer(handle.BlockSlot);
        }

        /// <summary>
        /// Get the description of the buffer
        /// </summary>
        [[nodiscard]] const auto& GetDesc() const
        {
            return m_Desc;
        }

    public:
        /// <summary>
        /// Rent a slot in the buffer
        /// </summary>
        [[nodiscard]] Handle Rent(
            size_t size,
            size_t alignment = 1)
        {
            return FindSlot(size, alignment);
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
            size_t size,
            size_t alignment)
        {
            Handle handle;
            for (uint32_t i = 0; i < m_Blocks.size(); i++)
            {
                auto& block       = m_Blocks[i];
                auto  foundHandle = block.Buddy.Allocate(size, alignment);

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
                handle = CreateBlockAndAllocateSlot(size, alignment);
            }

            return handle;
        }

        /// <summary>
        /// Create a new block in the buffer with the given size
        /// If the size is larger than the buffer, return an invalid handle
        /// </summary>
        [[nodiscard]] Handle CreateBlockAndAllocateSlot(
            size_t size,
            size_t alignment)
        {
            if (m_Desc.MaxBlockCount && m_Desc.MaxBlockCount <= m_Blocks.size()) [[unlikely]]
            {
                return {};
            }

            m_Desc.BuffDesc.Size = GetSuitableBlockSize(size);

            Dg::Ptr<Dg::IBuffer> buffer;
            m_Desc.Device->CreateBuffer(m_Desc.BuffDesc, nullptr, &buffer);
            if (m_Desc.OnBufferCreate)
            {
                m_Desc.OnBufferCreate(buffer);
            }

            auto& block  = m_Blocks.emplace_back(std::move(buffer), m_Desc.BuffDesc.Size);
            auto  handle = block.Buddy.Allocate(size, alignment);

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
            size_t blockSize = m_Desc.BuffDesc.Size;
            if (blockSize < size)
            {
                size_t factor = static_cast<size_t>(std::ceil(static_cast<double>(size) / blockSize));
                blockSize *= factor;
            }
            return blockSize;
        }

    private:
        void BlockSlotMustExists(
            uint32_t blockSlot) const
        {
#ifdef AME_DEBUG
            if (blockSlot >= m_Blocks.size()) [[unlikely]]
            {
                throw std::out_of_range("Block slot does not exist");
                std::unreachable();
            }
#endif
        }

    private:
        BlockBasedBufferDesc m_Desc;

        struct Block
        {
            Allocator::Buddy     Buddy;
            Dg::Ptr<Dg::IBuffer> Buffer;

            Block(
                Dg::Ptr<Dg::IBuffer> buffer,
                size_t               size) :
                Buddy(Allocator::Buddy(size)),
                Buffer(std::move(buffer))
            {
            }

            void Reset()
            {
                Buddy = Allocator::Buddy(Buffer->GetDesc().Size);
            }
        };

        std::vector<Block> m_Blocks;
    };
} // namespace Ame::Rhi::Util