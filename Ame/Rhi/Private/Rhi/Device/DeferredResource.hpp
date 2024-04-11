#pragma once

#include <EASTL/vector.h>
#include <mutex>
#include <algorithm>
#include <execution>

#include <Core/Ame.hpp>
#include <Rhi/Core.hpp>

#include <Rhi/Nri/Nri.hpp>

namespace Ame::Rhi
{
    template<typename Ty, typename ReleaseFunc>
    struct DeferredResource
    {
        eastl::vector<Ty*> Resources;
        std::mutex         Mutex;

        void DeferRelease(
            Ty& Resource)
        {
            std::scoped_lock Guard(Mutex);
            Resources.push_back(&Resource);
        }

        template<typename NriTy>
        void Release(
            NriTy& Nri)
        {
            std::scoped_lock Guard(Mutex);
            std::for_each(
                std::execution::par_unseq,
                Resources.begin(),
                Resources.end(),
                [&Nri](Ty* Resource)
                {
                    ReleaseFunc{}(Nri, *Resource);
                });
            Resources.clear();
        }
    };

    using DeferredBuffer = DeferredResource<
        nri::Buffer,
        decltype([](nri::CoreInterface& NriCore, nri::Buffer& Buffer)
                 { NriCore.DestroyBuffer(Buffer); })>;

    using DeferredTexture = DeferredResource<
        nri::Texture,
        decltype([](nri::CoreInterface& NriCore, nri::Texture& Texture)
                 { NriCore.DestroyTexture(Texture); })>;

    using DeferredDescriptor = DeferredResource<
        nri::Descriptor,
        decltype([](nri::CoreInterface& NriCore, nri::Descriptor& Descriptor)
                 { NriCore.DestroyDescriptor(Descriptor); })>;

    using DeferredPipeline = DeferredResource<
        nri::Pipeline,
        decltype([](nri::CoreInterface& NriCore, nri::Pipeline& Pipeline)
                 { NriCore.DestroyPipeline(Pipeline); })>;
} // namespace Ame::Rhi