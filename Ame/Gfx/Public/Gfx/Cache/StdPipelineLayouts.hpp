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
            m_Runtime(Runtime),
            m_Device(Device)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineLayout>> Get(
            Type LayoutType);

    private:
        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] static Rhi::PipelineLayoutDesc GetDesc(
            Type LayoutType);

    private:
        Ref<Co::runtime> m_Runtime;
        Ref<Rhi::Device> m_Device;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache