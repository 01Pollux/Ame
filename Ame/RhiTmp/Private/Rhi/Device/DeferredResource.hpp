#pragma once

#include <mutex>
#include <algorithm>
#include <execution>

#include <Rhi/Device/MemoryAllocator.hpp>

namespace Ame::Rhi
{
    template<typename Ty, typename ReleaseFunc>
    class DeferredResourceList
    {
    public:
        void DeferRelease(
            Ty& resource)
        {
            std::scoped_lock resourceLock(m_Mutex);
            m_Resources.push_back(&resource);
        }

        template<typename... ArgsTy>
        void Release(
            ArgsTy&&... args)
        {
            std::scoped_lock resourceLock(m_Mutex);
            std::for_each(
                std::execution::unseq,
                m_Resources.begin(),
                m_Resources.end(),
                [&](Ty* Resource)
                {
                    ReleaseFunc{}(std::forward<ArgsTy>(args)..., *Resource);
                });
            m_Resources.clear();
        }

    private:
        std::vector<Ty*> m_Resources;
        std::mutex       m_Mutex;
    };

    using DeferredBufferList = DeferredResourceList<
        nri::Buffer,
        decltype([](MemoryAllocator& memoryAllocator, nri::Buffer& nriBuffer)
                 { memoryAllocator.Release(&nriBuffer); })>;

    using DeferredTextureList = DeferredResourceList<
        nri::Texture,
        decltype([](MemoryAllocator& memoryAllocator, nri::Texture& nriTexture)
                 { memoryAllocator.Release(&nriTexture); })>;

    using DeferredDescriptorList = DeferredResourceList<
        nri::Descriptor,
        decltype([](nri::CoreInterface& nriCore, nri::Descriptor& nriDescriptor)
                 { nriCore.DestroyDescriptor(nriDescriptor); })>;

    using DeferredPipelineList = DeferredResourceList<
        nri::Pipeline,
        decltype([](nri::CoreInterface& nriCore, nri::Pipeline& nriPipeline)
                 { nriCore.DestroyPipeline(nriPipeline); })>;
} // namespace Ame::Rhi