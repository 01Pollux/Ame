#pragma once

#include <map>

#include <Core/Coroutine.hpp>
#include <Gfx/Shading/PropertyDescriptor.hpp>
#include <Rhi/Resource/Shader.hpp>

namespace Ame::Gfx::Cache
{
    class ShaderCache;
} // namespace Ame::Gfx::Cache

namespace Ame::Gfx::Shading
{
    class MaterialCommonState
    {
    public:
        using PipelineStateHash = std::array<uint8_t, 32>;
        using PipelineStateMap  = std::map<PipelineStateHash, Ptr<Rhi::PipelineState>>;

    public:
        MaterialCommonState(
            Rhi::Device&             RhiDevice,
            Gfx::Cache::ShaderCache& ShaderCache,
            Ptr<Rhi::PipelineLayout> PipelineLayout,
            MaterialPipelineState    PipelineState);

    public:
        [[nodiscard]] Rhi::Device& GetDevice() const;

        /// <summary>
        /// Get the pipeline layout for the material
        /// </summary>
        [[nodiscard]] Ptr<Rhi::PipelineLayout> GetPipelineLayout() const;

        /// <summary>
        /// Get the pipeline state for the given render state
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineState>> GetPipelineState(
            const MaterialRenderState& RenderState) const;

    private:
        /// <summary>
        /// Get the hash of the pipeline state
        /// </summary>
        [[nodiscard]] PipelineStateHash GetStateHash(
            const MaterialRenderState& RenderState) const;

        /// <summary>
        /// Create the pixel shader for the material
        /// </summary>
        [[nodiscard]] Co::result<Rhi::ShaderBytecode> CreatePixelShader(
            const MaterialRenderState& RenderState) const;

        /// <summary>
        /// Get shader descs for pipeline state
        /// </summary>
        [[nodiscard]] Co::result<std::vector<Rhi::ShaderDesc>> GetShaderDescs(
            const Rhi::ShaderBytecode& PixelShader) const;

        /// <summary>
        /// Create pipeline state for the material
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineState>> CreatePipelineState(
            const MaterialRenderState& RenderState) const;

    private:
        Ref<Rhi::Device>             m_RhiDevice;
        Ref<Gfx::Cache::ShaderCache> m_ShaderCache;

        Ptr<Rhi::PipelineLayout>     m_PipelineLayout;
        std::vector<Rhi::ShaderDesc> m_ShaderDescs;

        MaterialPipelineState    m_PipelineStateDesc;
        mutable PipelineStateMap m_PipelineStates;
    };
} // namespace Ame::Gfx::Shading