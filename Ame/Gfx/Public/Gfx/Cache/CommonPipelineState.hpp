#pragma once

#include <map>
#include <concurrencpp/concurrencpp.h>

#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Shader/Shader.hpp>

#include <Gfx/Cache/CommonPipelineLayout.hpp>
#include <Gfx/Cache/CommonShader.hpp>

namespace Ame::Asset
{
    class Storage;
} // namespace Ame::Asset

namespace Ame::Gfx::Cache
{
    class CommonPipelineState
    {
        using ShaderTaskStorage = std::vector<Co::result<Rhi::ShaderBytecode>>;
        using ShaderStorage     = std::vector<Rhi::ShaderBytecode>;
        using ShaderDescStorage = std::vector<Rhi::ShaderDesc>;

        struct ShaderTable
        {
            ShaderStorage     Shaders;
            ShaderDescStorage ShaderDescs;
        };

    public:
        enum class Type
        {
            EntityCollectPass,
            GBufferPass,

            Count
        };

        using TypedCacheList = std::array<Ptr<Rhi::ScopedPipelineState>, std::to_underlying(Type::Count)>;
        using CacheMap       = std::map<size_t, Ptr<Rhi::ScopedPipelineState>>;

    public:
        CommonPipelineState(
            Co::runtime&          coroutine,
            Rhi::Device&          rhiDevice,
            CommonPipelineLayout& commonLayouts,
            CommonShader&         commonShaders) :
            m_Runtime(coroutine),
            m_Device(rhiDevice),
            m_CommonLayouts(commonLayouts),
            m_CommonShaders(commonShaders)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::ScopedPipelineState>> Load(
            Type type);

        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::ScopedPipelineState>> Load(
            const Rhi::GraphicsPipelineDesc& desc);

        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::ScopedPipelineState>> Load(
            const Rhi::ComputePipelineDesc& desc);

    private:
        /// <summary>
        /// Preload the pipeline layout.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::ScopedPipelineLayout>> PrepareLayout(
            Type type);

        /// <summary>
        /// Preload the shaders.
        /// </summary>
        [[nodiscard]] ShaderTable PrepareShaders(
            Type type);

    private:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        [[nodiscard]] static CommonPipelineLayout::Type GetLayoutType(
            Type type);

        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] Ptr<Rhi::ScopedPipelineState> CreateByType(
            const Rhi::PipelineLayout& layout,
            const ShaderDescStorage&   shaderDescs,
            Type                       type);

    private:
        Ref<Co::runtime> m_Runtime;
        Ref<Rhi::Device> m_Device;

        Ref<CommonPipelineLayout> m_CommonLayouts;
        Ref<CommonShader>         m_CommonShaders;

        TypedCacheList m_TypedCaches;
        CacheMap       m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache