#pragma once

#include <map>
#include <concurrencpp/concurrencpp.h>

#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Resource/Shader.hpp>

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

            static Co::result<ShaderTable> Create(
                ShaderTaskStorage tasks);
        };

    public:
        enum class Type
        {
            EntityCollectPass,
            GBufferPass,

            Count
        };

        using CacheList = std::array<Ptr<Rhi::PipelineState>, std::to_underlying(Type::Count)>;

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
        Co::result<Ptr<Rhi::PipelineState>> Load(
            Type type);

    private:
        /// <summary>
        /// Preload the pipeline layout.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineLayout>> PrepareLayout(
            Type type);

        /// <summary>
        /// Preload the shaders.
        /// </summary>
        [[nodiscard]] Co::result<ShaderTaskStorage> PrepareShaders(
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
        [[nodiscard]] static Co::result<Ptr<Rhi::PipelineState>> Create(
            Rhi::Device&                    rhiDevice,
            Co::executor&                   executor,
            const Ptr<Rhi::PipelineLayout>& layout,
            const ShaderDescStorage&        shaderDescs,
            Type                            type);

    private:
        Ref<Co::runtime> m_Runtime;
        Ref<Rhi::Device> m_Device;

        Ref<CommonPipelineLayout> m_CommonLayouts;
        Ref<CommonShader>         m_CommonShaders;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache