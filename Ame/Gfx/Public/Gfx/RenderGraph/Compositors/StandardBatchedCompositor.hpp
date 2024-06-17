#pragma once

#include <Gfx/Compositor/Signals.hpp>
#include <Gfx/Cache/FrameBufferCache.hpp>

namespace Ame::Gfx
{
    class StandardBatchedCompositor
    {
    public:
        class StandardBatchedCompositor(
            EntityCompositor&                      entityCompositor,
            Cache::FrameDynamicBufferCache<false>& bufferCache);

    private:
        void OnRenderCompose(
            Signals::Data::DrawCompositorData& renderData);

    private:
        Ref<Cache::FrameDynamicBufferCache<false>> m_BufferCache;

        Signals::ScopedConnection m_OnRenderCompose;
    };
} // namespace Ame::Gfx