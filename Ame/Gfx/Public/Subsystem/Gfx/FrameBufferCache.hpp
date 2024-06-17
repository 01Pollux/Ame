#pragma once

#include <Core/Subsystem.hpp>

#include <Subsystem/Core/Frame.hpp>
#include <Subsystem/Rhi/Device.hpp>

#include <Gfx/Cache/FrameBufferCache.hpp>

namespace Ame::Gfx::Cache
{
    template<typename Ty>
    struct FrameBufferCacheSubsystem : SingleSubsystem<
                                           FrameBufferCache<Ty>,
                                           Dependency<
                                               EngineFrameSubsystem,
                                               Rhi::DeviceSubsystem>>,
                                       kgr::final
    {
    };

    template<typename Ty>
    auto service_map(const FrameBufferCache<Ty>&) -> FrameBufferCacheSubsystem<Ty>;

    //

    template<bool WithStreaming = false>
    using FrameDynamicBufferCacheSubsystem = FrameBufferCacheSubsystem<Rhi::Util::BlockBasedBuffer<WithStreaming>>;
    template<typename Ty, bool WithStreaming = false>
    using FrameSlotBufferCacheSubsystem = FrameBufferCacheSubsystem<Rhi::Util::SlotBasedBuffer<Ty, WithStreaming>>;
} // namespace Ame::Gfx::Cache
