#pragma once

#include <map>
#include <concurrencpp/concurrencpp.h>

#include <Rhi/Resource/PipelineState.hpp>

namespace Ame::Gfx::Cache
{
    class PipelineLayoutCache;
    class ShaderCache;

    class PipelineStateCache
    {
    public:
        enum class Type
        {
            EntityCollectPass,

            Count
        };

        using CacheList = std::array<Ptr<Rhi::PipelineState>, std::to_underlying(Type::Count)>;

    public:
        PipelineStateCache(
            Rhi::Device&         Device,
            Co::runtime&         Runtime,
            PipelineLayoutCache& LayoutCache,
            ShaderCache&         ShaderCache) :
            m_Device(Device),
            m_Runtime(Runtime),
            m_LayoutCache(LayoutCache),
            m_ShaderCache(ShaderCache)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::PipelineLayout>> Load(
            Type LayoutType);

    private:
        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] static Co::result<Ptr<Rhi::PipelineState>> Create(
            Rhi::Device&  Device,
            Co::executor& Executor,
            Type          LayoutType);

    private:
        Ref<Rhi::Device> m_Device;
        Ref<Co::runtime> m_Runtime;

        Ref<PipelineLayoutCache> m_LayoutCache;
        Ref<ShaderCache>         m_ShaderCache;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache