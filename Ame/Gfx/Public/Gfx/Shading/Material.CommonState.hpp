#pragma once

#include <map>

#include <Core/Coroutine.hpp>
#include <Gfx/Shading/PropertyDescriptor.hpp>
#include <Rhi/Resource/Shader.hpp>

namespace Ame::Gfx::Shading
{
    class MaterialCommonState
    {
    public:
        using PipelineStateHash = uint64_t;
        using PipelineStateMap  = std::map<PipelineStateHash, Ptr<Rhi::PipelineState>>;

    public:
        MaterialCommonState(
            Rhi::Device&                 RhiDevice,
            Ptr<Rhi::PipelineLayout>     PipelineLayout,
            const MaterialPipelineState& PipelineState);

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
        [[nodiscard]] static PipelineStateHash GetStateHash(
            const MaterialRenderState& RenderState);

        [[nodiscard]] static Co::result<Ptr<Rhi::PipelineState>> CreatePipelineState(
            Rhi::Device&                 RhiDevice,
            Ptr<Rhi::PipelineLayout>     PipelineLayout,
            const MaterialPipelineState& PipelineState,
            const MaterialRenderState&   RenderState);

    private:
        Ref<Rhi::Device> m_RhiDevice;

        Ptr<Rhi::PipelineLayout>     m_PipelineLayout;
        std::vector<Rhi::ShaderDesc> m_ShaderDescs;

        MaterialPipelineState    m_PipelineStateDesc;
        mutable PipelineStateMap m_PipelineStates;
    };
} // namespace Ame::Gfx::Shading