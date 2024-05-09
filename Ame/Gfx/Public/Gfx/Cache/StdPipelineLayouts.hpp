#pragma once

#include <map>
#include <concurrencpp/concurrencpp.h>

#include <Rhi/Resource/PipelineLayout.hpp>

namespace Ame::Gfx::Cache
{
    class PipelineLayoutCache
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
        PipelineLayoutCache(
            Rhi::Device& Device,
            Co::runtime& Runtime) :
            m_Device(Device),
            m_Runtime(Runtime)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineLayout>> Load(
            Type LayoutType);

    private:
        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] static Co::result<Ptr<Rhi::PipelineLayout>> Create(
            Rhi::Device&  Device,
            Co::executor& Executor,
            Type          LayoutType);

    private:
        Ref<Rhi::Device> m_Device;
        Ref<Co::runtime> m_Runtime;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache