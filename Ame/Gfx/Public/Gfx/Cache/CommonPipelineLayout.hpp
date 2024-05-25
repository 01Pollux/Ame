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

        using CacheList     = std::array<Ptr<Rhi::PipelineLayout>, std::to_underlying(Type::Count)>;
        using CacheLoadList = std::array<Co::result<void>, std::to_underlying(Type::Count)>;

    public:
        CommonPipelineLayout(
            Rhi::Device& rhiDevice,
            Co::runtime& coroutine) :
            m_Device(rhiDevice),
            m_Runtime(coroutine)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::PipelineLayout>> Load(
            Type type);

    private:
        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] static Co::result<Ptr<Rhi::PipelineLayout>> Create(
            Rhi::Device&  rhiDevice,
            Co::executor& executor,
            Type          type);

    private:
        Ref<Rhi::Device> m_Device;
        Ref<Co::runtime> m_Runtime;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache