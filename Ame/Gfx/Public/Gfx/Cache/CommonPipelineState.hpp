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
                ShaderTaskStorage Tasks);
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
            Rhi::Device&          Device,
            Co::runtime&          Runtime,
            CommonPipelineLayout& CommonLayouts,
            CommonShader&         CommonShaders) :
            m_Device(Device),
            m_Runtime(Runtime),
            m_CommonLayouts(CommonLayouts),
            m_CommonShaders(CommonShaders)
        {
        }

    public:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        Co::result<Ptr<Rhi::PipelineState>> Load(
            Type PipelineType);

    private:
        /// <summary>
        /// Preload the pipeline layout.
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineLayout>> PrepareLayout(
            Type PipelineType);

        /// <summary>
        /// Preload the shaders.
        /// </summary>
        [[nodiscard]] Co::result<ShaderTaskStorage> PrepareShaders(
            Type PipelineType);

    private:
        /// <summary>
        /// Load or get a pipeline layout from cache.
        /// </summary>
        [[nodiscard]] static CommonPipelineLayout::Type GetLayoutType(
            Type PipelineType);

        /// <summary>
        /// Get the pipeline layout desc.
        /// </summary>
        [[nodiscard]] static Co::result<Ptr<Rhi::PipelineState>> Create(
            Rhi::Device&                    Device,
            Co::executor&                   Executor,
            const Ptr<Rhi::PipelineLayout>& Layout,
            const ShaderDescStorage&        ShaderDescs,
            Type                            PipelineType);

    private:
        Ref<Rhi::Device> m_Device;
        Ref<Co::runtime> m_Runtime;

        Ref<CommonPipelineLayout> m_CommonLayouts;
        Ref<CommonShader>         m_CommonShaders;

        CacheList      m_Caches;
        Co::async_lock m_Mutex;
    };
} // namespace Ame::Gfx::Cache