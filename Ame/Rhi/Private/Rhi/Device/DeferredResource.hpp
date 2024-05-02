#pragma once

#include <mutex>
#include <algorithm>
#include <execution>

#include <Rhi/Device/MemoryAllocator.hpp>

namespace Ame::Rhi
{
    template<typename Ty, typename ReleaseFunc>
    struct DeferredResource
    {
        std::vector<Ty*> Resources;
        std::mutex       Mutex;

        void DeferRelease(
            Ty& Resource)
        {
            std::scoped_lock Guard(Mutex);
            Resources.push_back(&Resource);
        }

        template<typename... ArgsTy>
        void Release(
            ArgsTy&&... Args)
        {
            std::scoped_lock Guard(Mutex);
            std::for_each(
                std::execution::unseq,
                Resources.begin(),
                Resources.end(),
                [&](Ty* Resource)
                {
                    ReleaseFunc{}(std::forward<ArgsTy>(Args)..., *Resource);
                });
            Resources.clear();
        }
    };

    using DeferredBuffer = DeferredResource<
        nri::Buffer,
        decltype([](MemoryAllocator& MemAllocator, nri::Buffer& Buffer)
                 { MemAllocator.Release(&Buffer); })>;

    using DeferredTexture = DeferredResource<
        nri::Texture,
        decltype([](MemoryAllocator& MemAllocator, nri::Texture& Texture)
                 { MemAllocator.Release(&Texture); })>;

    using DeferredDescriptor = DeferredResource<
        nri::Descriptor,
        decltype([](nri::CoreInterface& NriCore, nri::Descriptor& Descriptor)
                 { NriCore.DestroyDescriptor(Descriptor); })>;

    using DeferredPipeline = DeferredResource<
        nri::Pipeline,
        decltype([](nri::CoreInterface& NriCore, nri::Pipeline& Pipeline)
                 { NriCore.DestroyPipeline(Pipeline); })>;
} // namespace Ame::Rhi