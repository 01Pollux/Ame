#pragma once

#include <map>
#include <Shading/Core.hpp>

namespace Ame::Rhi
{
    /// <summary>
    /// Class that defines material's pipeline state and shaders
    /// </summary>
    class MaterialTechnique
    {
    private:
        using PipelineStateMap = std::map<MaterialHash, Ptr<Dg::IPipelineState>>;

    public:
        MaterialTechnique(
            Dg::IRenderDevice*         renderDevice,
            const MaterialRenderState& renderState);

    public:
        /// <summary>
        /// Get the pipeline state for the given material and vertex input flags
        /// </summary>
        [[nodiscard]] Dg::IPipelineState* GetPipelineState(
            MaterialVertexInputFlags vertexInputFlags,
            const Material*          material) const;

    private:
        using ShadersToKeepAliveList = std::vector<Ptr<Dg::IShader>>;
        using SignaturesToKeepAlive  = std::vector<Dg::IPipelineResourceSignature*>;

    private:
        /// <summary>
        /// Get combined shaders for pipeline state
        /// </summary>
        [[nodiscard]] ShadersToKeepAliveList CombineShaders(
            Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
            const MaterialDesc&                  materialDesc) const;

        /// <summary>
        /// Get combined signatures for pipeline state
        /// </summary>
        [[nodiscard]] SignaturesToKeepAlive CombineSignatures(
            Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
            const Material*                      material) const;

        /// <summary>
        /// Initialize pipeline state for the material
        /// </summary>
        void InitializePipelineState(
            Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
            const MaterialDesc&                  materialDesc) const;

        /// <summary>
        /// Create pipeline state for the material
        /// </summary>
        [[nodiscard]] Ptr<Dg::IPipelineState> CreatePipelineState(
            MaterialVertexInputFlags vertexInputFlags,
            const Material*          material) const;

    private:
        Dg::IRenderDevice* m_RenderDevice = nullptr;

        MaterialRenderState      m_RenderState;
        mutable PipelineStateMap m_PipelineStates;
    };
} // namespace Ame::Rhi