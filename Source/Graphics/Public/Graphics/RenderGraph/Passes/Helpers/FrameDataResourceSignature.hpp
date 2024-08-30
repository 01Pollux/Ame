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
            Execute(std::bind_front(&FrameDataResourceSignature::OnExecute, this));
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

            resolver.SetUserData(Ty::RGSignature(), m_Srb);
        }

        void OnExecute(
            const Rg::ResourceStorage& storage,
            Dg::IDeviceContext*)
        {
            auto buffer            = storage.GetResource(c_RGFrameData)->AsBuffer();
            auto resourceSignature = m_Srb->GetPipelineResourceSignature();

            Rhi::BindAllStaticInSignature(resourceSignature, ShaderFlags, "FrameDataBuffer", buffer);
            resourceSignature->InitializeStaticSRBResources(m_Srb);

            Execute(nullptr);
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
        FrameDataResourceSignature_GraphicsPass()
        {
            Name("Initialize FrameData Resource Signature (Graphics)");
        }

        [[nodiscard]] static const auto& RGSignature() noexcept
        {
            return c_RGFrameDataResourceSignature_Graphics;
        }
    };

    class FrameDataResourceSignature_ComputePass final
        : public FrameDataResourceSignature<FrameDataResourceSignature_ComputePass, Dg::SHADER_TYPE_COMPUTE>
    {
    public:
        FrameDataResourceSignature_ComputePass()
        {
            Name("Initialize FrameData Resource Signature (Compute)");
        }

        [[nodiscard]] static const auto& RGSignature() noexcept
        {
            return c_RGFrameDataResourceSignature_Compute;
        }
    };
} // namespace Ame::Gfx