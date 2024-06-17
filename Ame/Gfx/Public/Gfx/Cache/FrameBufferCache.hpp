#pragma once

#include <Frame/EngineFrame.hpp>
#include <Rhi/Util/BlockBasedBuffer.hpp>
#include <Rhi/Util/SlotBasedBuffer.hpp>

namespace Ame::Gfx::Cache
{
    template<typename BufferAllocatorTy>
    class FrameBufferCache : public BufferAllocatorTy
    {
    public:
        using typename BufferAllocatorTy::DescType;

    public:
        FrameBufferCache(
            EngineFrame&    engineFrame,
            Rhi::Device&    rhiDevice,
            const DescType& desc = { .UsageFlags = Rhi ::BufferUsageBits::VERTEX_BUFFER |
                                                   Rhi ::BufferUsageBits::INDEX_BUFFER }) :
            m_EndFrameHandle(engineFrame.OnEndFrame({ &BufferAllocatorTy::Reset, this })),
            BufferAllocatorTy(rhiDevice, desc)
        {
        }

    private:
        Signals::ScopedConnection m_EndFrameHandle;
    };

    //

    template<bool WithStreaming = false>
    using FrameDynamicBufferCache = FrameBufferCache<Rhi::Util::BlockBasedBuffer<WithStreaming>>;
    template<typename Ty, bool WithStreaming = false>
    using FrameSlotBufferCache = FrameBufferCache<Rhi::Util::SlotBasedBuffer<Ty, WithStreaming>>;
} // namespace Ame::Gfx::Cache