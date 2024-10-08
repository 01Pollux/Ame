#pragma once

#include <map>
#include <Shading/Core.hpp>

namespace Ame::Rhi
{
    // {2A8E47FC-F038-4C7A-B090-1117A595C68A}
    inline constexpr UId IID_MaterialTechnique = {
        0x2a8e47fc, 0xf038, 0x4c7a, { 0xb0, 0x90, 0x11, 0x17, 0xa5, 0x95, 0xc6, 0x8a }
    };

    /// <summary>
    /// Class that defines material's pipeline state and shaders
    /// </summary>
    class AME_ENGINE_API MaterialTechnique : public IObjectWithCallback
    {
    private:
        using PipelineStateMap = std::map<MaterialHash, Ptr<Dg::IPipelineState>>;
        using SRBMap           = std::map<MaterialHash, Ptr<Dg::IShaderResourceBinding>>;

    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_MaterialTechnique, IObjectWithCallback);

    private:
        IMPLEMENT_INTERFACE_CTOR(MaterialTechnique, Dg::IRenderDevice* renderDevice, MaterialRenderState renderState);

    public:
        /// <summary>
        /// Get the pipeline state for the given material and vertex input flags
        /// </summary>
        [[nodiscard]] Dg::IPipelineState* GetPipelineState(const MaterialVertexDesc&    vertexDesc,
                                                           const Material*              material,
                                                           Dg::IShaderResourceBinding** srb = nullptr) const;

    private:
        using ShadersToKeepAliveList = std::vector<Ptr<Dg::IShader>>;
        using SignaturesToKeepAlive  = std::vector<Dg::IPipelineResourceSignature*>;

    private:
        /// <summary>
        /// Get combined shaders for pipeline state
        /// </summary>
        [[nodiscard]] ShadersToKeepAliveList CombineShaders(Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
                                                            const Material*                      material) const;

        /// <summary>
        /// Get combined signatures for pipeline state
        /// </summary>
        [[nodiscard]] SignaturesToKeepAlive CombineSignatures(Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
                                                              const Material*                      material) const;

        /// <summary>
        /// Initialize pipeline state for the material
        /// </summary>
        void InitializePipelineState(Dg::GraphicsPipelineStateCreateInfo& graphicsPsoDesc,
                                     const MaterialVertexDesc&            vertexDesc) const;

        /// <summary>
        /// Create pipeline state for the material
        /// </summary>
        [[nodiscard]] Ptr<Dg::IPipelineState> CreatePipelineState(const MaterialVertexDesc& vertexDesc,
                                                                  const Material*           material) const;

    private:
        Dg::IRenderDevice* m_RenderDevice = nullptr;

        MaterialRenderState      m_RenderState;
        mutable PipelineStateMap m_PipelineStates;
        mutable SRBMap           m_SRBs;
    };
} // namespace Ame::Rhi