#pragma once

#include <RG/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Rhi/Utils/SRBBinder.hpp>

namespace Ame::Gfx
{
    template<typename Ty, Dg::SHADER_TYPE ShaderFlags>
    class Initialize_EntityResourceSignature : public RG::Pass
    {
    public:
        Initialize_EntityResourceSignature()
        {
            Build(std::bind_front(&Initialize_EntityResourceSignature::OnBuild, this));
            Execute(std::bind_front(&Initialize_EntityResourceSignature::OnExecute, this));
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
            auto renderInstances = storage.GetResource(c_RGRenderInstances)->AsBuffer();

            if (!m_StaticInitialized)
            {
                m_StaticInitialized = true;
                auto signature      = m_Srb->GetPipelineResourceSignature();
                Rhi::BindAllStaticInSignature(signature, ShaderFlags, "FrameDataBuffer", frameData->Resource);
                signature->InitializeStaticSRBResources(m_Srb);
            }
            Rhi::BindAllInSrb(m_Srb, ShaderFlags, "RenderInstances", renderInstances->Resource->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
        }

    private:
        [[nodiscard]] static Ptr<Dg::IPipelineResourceSignature> CreateResourceSignature(
            Dg::IRenderDevice* renderDevice)
        {
            Dg::PipelineResourceDesc resources[]{
                Dg::PipelineResourceDesc{ ShaderFlags, "FrameDataBuffer", Dg::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER, Dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC },
                Dg::PipelineResourceDesc{ ShaderFlags, "RenderInstances", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV, Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
            };

            Dg::PipelineResourceSignatureDesc desc{
                .Resources    = resources,
                .NumResources = Rhi::Count32(resources),
                .BindingIndex = 1
            };

            Ptr<Dg::IPipelineResourceSignature> signature;
            renderDevice->CreatePipelineResourceSignature(desc, &signature);

            return signature;
        }

    private:
        Ptr<Dg::IShaderResourceBinding> m_Srb;
        bool                            m_StaticInitialized = false;
    };

    //

    class Initialize_EntityResourceSignature_Graphics final
        : public Initialize_EntityResourceSignature<Initialize_EntityResourceSignature_Graphics, Dg::SHADER_TYPE_ALL_GRAPHICS>
    {
    public:
        static inline const RG::ResourceId RGEntityResourceSignature{ "_ERS_Graphics" };

    public:
        Initialize_EntityResourceSignature_Graphics()
        {
            Name("Initialize Entity Resource Signature (Graphics)");
        }
    };
} // namespace Ame::Gfx