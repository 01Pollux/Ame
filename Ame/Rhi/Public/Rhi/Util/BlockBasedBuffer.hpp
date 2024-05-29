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
        struct Handle
        {
            static constexpr uint32_t c_InvalidValue   = std::numeric_limits<uint32_t>::max();
            static constexpr uint64_t c_InvalidValue64 = std::numeric_limits<uint64_t>::max();

            uint64_t FrameIndex = c_InvalidValue64;
            uint32_t BlockSlot  = c_InvalidValue;
            uint32_t Offset     = c_InvalidValue;
            uint32_t Size       = c_InvalidValue;

            operator bool() const
            {
                return BlockSlot != c_InvalidValue;
            }

            [[nodiscard]] auto operator<=>(
                const Handle& other) const
            {
                return FrameIndex <=> other.FrameIndex;
            }
        };

        static constexpr Handle c_InvalidHandle = {};

    private:
        using BusyHandleSet = boost::container::flat_set<Allocator::Buddy::Handle>;
        using FreeHandleSet = boost::container::flat_multiset<Handle>;
        using BufferStream  = UPtr<Streaming::BufferOStream>;

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
            Handle slot)
        {
            slot.FrameIndex = m_Device.get().GetFrameCount();
            m_DiscardedHandles.insert(slot);
        }

    public:
        void Reset()
        {
            uint64_t currentFrameIndex = m_Device.get().GetFrameCount();
            for (uint32_t i = 0; i < m_Blocks.size(); i++)
            {
                for (auto& handle : m_Blocks[i].BusyHandles)
                {
                    m_DiscardedHandles.insert(Handle{
                        .FrameIndex = currentFrameIndex,
                        .BlockSlot  = i,
                        .Offset     = static_cast<uint32_t>(handle.Offset),
                        .Size       = static_cast<uint32_t>(handle.Size) });
                }
                m_Blocks[i].BusyHandles.clear();
            }
        }

    private:
        /// <summary>
        /// Find a slot in the buffer with the given size, if not found, create a new block
        /// </summary>
        [[nodiscard]] Handle FindSlot(
            uint32_t size)
        {
            // If the size is larger than the buffer, return an invalid handle
            if (size > Handle::c_InvalidValue) [[unlikely]]
            {
                std::unreachable();
            }

            uint64_t currentFrameIndex = m_Device.get().GetFrameCount();
            Handle   handle;

            DiscardFrames(currentFrameIndex);

            for (uint32_t i = 0; i < m_Blocks.size(); i++)
            {
                auto& block       = m_Blocks[i];
                auto  foundHandle = block.Buddy.Allocate(size);

                if (foundHandle)
                {
                    block.BusyHandles.insert(foundHandle);
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

            block.BusyHandles.emplace(handle);
            return {
                .BlockSlot = static_cast<uint32_t>(m_Blocks.size() - 1),
                .Offset    = static_cast<uint32_t>(handle.Offset),
                .Size      = static_cast<uint32_t>(handle.Size)
            };
        }

        /// <summary>
        /// Flush all frames that are older than the given frame index + frame count
        /// </summary>
        void DiscardFrames(
            uint64_t currentFrameIndex)
        {
            uint8_t frameCount = m_Device.get().GetFrameCountInFlight();
            if (frameCount > currentFrameIndex) [[unlikely]]
            {
                return;
            }

            currentFrameIndex -= frameCount;

            auto it = m_DiscardedHandles.begin();
            while (it != m_DiscardedHandles.end())
            {
                if (it->FrameIndex <= currentFrameIndex)
                {
                    auto& block = m_Blocks[it->BlockSlot];
                    block.Buddy.Free(
                        { .Offset = it->Offset,
                          .Size   = it->Size });
                    block.BusyHandles.erase({ it->Offset, it->Size });

                    it = m_DiscardedHandles.erase(it);
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
        FreeHandleSet m_DiscardedHandles;
    };
} // namespace Ame::Rhi::Util