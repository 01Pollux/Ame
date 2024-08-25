#pragma once

#include <Rg/Pass.hpp>
#include <Rhi/Utils/SRBBinder.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>

namespace Ame::Gfx
{
    struct LightingDrawProp
    {
        Dg::IBufferView*  LinkedLightIndices = nullptr;
        Dg::ITextureView* LightGrid          = nullptr;
    };

    template<typename Ty, Dg::SHADER_TYPE ShaderFlags>
    class LightingResourceSignaturePass : public Rg::Pass
    {
    public:
        LightingResourceSignaturePass()
        {
            Build(std::bind_front(&LightingResourceSignaturePass::OnBuild, this));
        }

        static void Bind(
            Dg::IShaderResourceBinding* srb,
            const LightingDrawProp&     prop)
        {
            Rhi::BindAllInSrb(srb, ShaderFlags, "LinkedLightIndices", prop.LinkedLightIndices);
            Rhi::BindAllInSrb(srb, ShaderFlags, "LightGrid", prop.LightGrid);
        }

    private:
        void OnBuild(
            Rg::Resolver& resolver)
        {
            if (!m_Srb)
            {
                auto renderDevice = resolver.GetDevice()->GetRenderDevice();

                Ptr resourceSignature = CreateResourceSignature(renderDevice);
                resourceSignature->CreateShaderResourceBinding(&m_Srb);
            }

            resolver.SetUserData(Ty::RGEntityResourceSignature, m_Srb);
        }

    private:
        [[nodiscard]] static Ptr<Dg::IPipelineResourceSignature> CreateResourceSignature(
            Dg::IRenderDevice* renderDevice)
        {
            constexpr std::array resources{
                Dg::PipelineResourceDesc{ ShaderFlags, "LinkedLightIndices", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ ShaderFlags, "LightGrid", Dg::SHADER_RESOURCE_TYPE_TEXTURE_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
            };

            Dg::PipelineResourceSignatureDesc desc{
                .Resources    = resources.data(),
                .NumResources = Rhi::Count32(resources),
                .BindingIndex = 1
            };

            Ptr<Dg::IPipelineResourceSignature> signature;
            renderDevice->CreatePipelineResourceSignature(desc, &signature);

            return signature;
        }

    private:
        Ptr<Dg::IShaderResourceBinding> m_Srb;
    };

    //

    class LightingResourceSignaturePass_GraphicsPass final
        : public LightingResourceSignaturePass<LightingResourceSignaturePass_GraphicsPass, Dg::SHADER_TYPE_ALL_GRAPHICS>
    {
    public:
        static inline const Rg::ResourceId RGEntityResourceSignature = c_RGLightingResourceSignature_Graphics;

    public:
        LightingResourceSignaturePass_GraphicsPass()
        {
            Name("Initialize Lighting Resource Signature (Graphics)");
        }
    };

    class LightingResourceSignaturePass_ComputePass final
        : public LightingResourceSignaturePass<LightingResourceSignaturePass_ComputePass, Dg::SHADER_TYPE_COMPUTE>
    {
    public:
        static inline const Rg::ResourceId RGEntityResourceSignature = c_RGLightingResourceSignature_Compute;

    public:
        LightingResourceSignaturePass_ComputePass()
        {
            Name("Initialize Lighting Resource Signature (Compute)");
        }
    };
} // namespace Ame::Gfx