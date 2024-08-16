#pragma once

#include <RG/Pass.hpp>
#include <Rhi/Utils/SRBBinder.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>

namespace Ame::Gfx
{
    template<typename Ty, Dg::SHADER_TYPE ShaderFlags, bool WithSamplers = true>
    class EntityResourceSignaturePass : public RG::Pass
    {
    public:
        EntityResourceSignaturePass()
        {
            Build(std::bind_front(&EntityResourceSignaturePass::OnBuild, this));
            Execute(std::bind_front(&EntityResourceSignaturePass::OnExecute, this));
        }

    private:
        void OnBuild(
            RG::Resolver& resolver)
        {
            if (!m_Srb)
            {
                auto renderDevice = resolver.GetDevice()->GetRenderDevice();

                Ptr resourceSignature = CreateResourceSignature(renderDevice);
                resourceSignature->CreateShaderResourceBinding(&m_Srb);
            }

            resolver.WriteUserData(Ty::RGEntityResourceSignature, m_Srb);
        }

        void OnExecute(
            const RG::ResourceStorage& storage,
            Dg::IDeviceContext*)
        {
            auto frameData       = storage.GetResource(c_RGFrameData)->AsBuffer();
            auto transforms      = storage.GetResource(c_RGTransformTable)->AsBuffer();
            auto renderInstances = storage.GetResource(c_RGRenderInstanceTable)->AsBuffer();

            if (!m_StaticInitialized)
            {
                m_StaticInitialized = true;
                auto signature      = m_Srb->GetPipelineResourceSignature();
                Rhi::BindAllStaticInSignature(signature, ShaderFlags, "FrameDataBuffer", frameData->Resource);
                signature->InitializeStaticSRBResources(m_Srb);
            }
            Rhi::BindAllInSrb(m_Srb, ShaderFlags, "Transforms", transforms->Resource->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
            Rhi::BindAllInSrb(m_Srb, ShaderFlags, "RenderInstances", renderInstances->Resource->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
        }

    private:
        [[nodiscard]] static Ptr<Dg::IPipelineResourceSignature> CreateResourceSignature(
            Dg::IRenderDevice* renderDevice)
        {
            constexpr Dg::PipelineResourceDesc resources[]{
                Dg::PipelineResourceDesc{ ShaderFlags, "FrameDataBuffer", Dg::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER, Dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
                Dg::PipelineResourceDesc{ ShaderFlags, "Transforms", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ ShaderFlags, "RenderInstances", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
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
                .Resources    = resources,
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
        bool                            m_StaticInitialized = false;
    };

    //

    class EntityResourceSignature_GraphicsPass final
        : public EntityResourceSignaturePass<EntityResourceSignature_GraphicsPass, Dg::SHADER_TYPE_ALL_GRAPHICS>
    {
    public:
        static inline const RG::ResourceId RGEntityResourceSignature = c_RGEntityResourceSignature_Graphics;

    public:
        EntityResourceSignature_GraphicsPass()
        {
            Name("Initialize Entity Resource Signature (Graphics)");
        }
    };
} // namespace Ame::Gfx