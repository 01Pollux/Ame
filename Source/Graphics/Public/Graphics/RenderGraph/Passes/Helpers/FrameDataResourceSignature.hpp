#pragma once

#include <Rg/Pass.hpp>
#include <Rhi/Utils/SRBBinder.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>

namespace Ame::Gfx
{
    template<typename Ty, Dg::SHADER_TYPE ShaderFlags, bool WithSamplers = true>
    class FrameDataResourceSignature : public Rg::Pass
    {
    public:
        FrameDataResourceSignature()
        {
            Build(std::bind_front(&FrameDataResourceSignature::OnBuild, this));
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

                Rhi::BindAllStaticInSignature(resourceSignature, ShaderFlags, "FrameDataBuffer", resolver.GetBuffer(c_RGFrameData));
                resourceSignature->InitializeStaticSRBResources(m_Srb);
            }

            resolver.SetUserData(Ty::RGEntityResourceSignature, m_Srb);
        }

    private:
        [[nodiscard]] static Ptr<Dg::IPipelineResourceSignature> CreateResourceSignature(
            Dg::IRenderDevice* renderDevice)
        {
            constexpr std::array resources{
                Dg::PipelineResourceDesc{ ShaderFlags, "FrameDataBuffer", Dg::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER, Dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
            };

            constexpr std::array c_SamplersTemplate{
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_LinearClamp", Dg::Sam_LinearClamp },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_PointClamp", Dg::Sam_PointClamp },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_LinearMirror", Dg::Sam_LinearMirror },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_PointWrap", Dg::Sam_PointWrap },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_LinearWrap", Dg::Sam_LinearWrap },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_Aniso2xClamp", Dg::Sam_Aniso2xClamp },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_Aniso4xClamp", Dg::Sam_Aniso4xClamp },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_Aniso2xWrap", Dg::Sam_Aniso2xWrap },
                Dg::ImmutableSamplerDesc{ ShaderFlags, "Sampler_Aniso4xWrap", Dg::Sam_Aniso4xWrap },
            };

            Dg::PipelineResourceSignatureDesc desc{
                .Resources    = resources.data(),
                .NumResources = Rhi::Count32(resources),
                .BindingIndex = 1
            };

            if constexpr (WithSamplers)
            {
                desc.ImmutableSamplers    = c_SamplersTemplate.data();
                desc.NumImmutableSamplers = Rhi::Count32(c_SamplersTemplate);
            }

            Ptr<Dg::IPipelineResourceSignature> signature;
            renderDevice->CreatePipelineResourceSignature(desc, &signature);

            return signature;
        }

    private:
        Ptr<Dg::IShaderResourceBinding> m_Srb;
    };

    //

    class FrameDataResourceSignature_GraphicsPass final
        : public FrameDataResourceSignature<FrameDataResourceSignature_GraphicsPass, Dg::SHADER_TYPE_ALL_GRAPHICS>
    {
    public:
        static inline const Rg::ResourceId RGEntityResourceSignature = c_RGFrameDataResourceSignature_Graphics;

    public:
        FrameDataResourceSignature_GraphicsPass()
        {
            Name("Initialize FrameData Resource Signature (Graphics)");
        }
    };

    class FrameDataResourceSignature_ComputePass final
        : public FrameDataResourceSignature<FrameDataResourceSignature_ComputePass, Dg::SHADER_TYPE_COMPUTE>
    {
    public:
        static inline const Rg::ResourceId RGEntityResourceSignature = c_RGFrameDataResourceSignature_Compute;

    public:
        FrameDataResourceSignature_ComputePass()
        {
            Name("Initialize FrameData Resource Signature (Compute)");
        }
    };
} // namespace Ame::Gfx