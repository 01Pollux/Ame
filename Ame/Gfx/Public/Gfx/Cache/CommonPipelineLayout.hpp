#pragma once

#include <map>
#include <concurrencpp/concurrencpp.h>

#include <Rhi/Resource/PipelineLayout.hpp>

namespace Ame::Gfx::Cache
{
    class CommonPipelineLayout
    {
    public:
        enum class Type
        {
            EntityCollectPass,

            Count
        };

        using TypedCacheList = std::array<Ptr<Rhi::ScopedPipelineLayout>, std::to_underlying(Type::Count)>;
        using CacheMap       = std::map<size_t, Ptr<Rhi::ScopedPipelineLayout>>;

    public:
        CommonPipelineLayout(
            Co::runtime& coroutine,
            Rhi::Device& rhiDevice) :
            m_Runtime(coroutine),
            m_Device(rhiDevice)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::ScopedPipelineLayout>> Load(
            Type type);

        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::ScopedPipelineLayout>> Load(
            const Rhi::PipelineLayoutDesc& layoutDesc);

    private:
        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]]  Co::result<Ptr<Rhi::ScopedPipelineLayout>> CreateByType(
            Type type);

    private:
        Ref<Co::runtime> m_Runtime;
        Ref<Rhi::Device> m_Device;

        TypedCacheList m_TypedCaches;
        CacheMap       m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache