#pragma once

#include <Rg/Pass.hpp>
#include <Rhi/Utils/SRBBinder.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>

namespace Ame::Gfx
{
    template<typename Ty, Dg::SHADER_TYPE ShaderFlags> class EntityResourceSignaturePass : public Rg::Pass
    {
    public:
        EntityResourceSignaturePass()
        {
            Build(std::bind_front(&EntityResourceSignaturePass::OnBuild, this));
            Execute(std::bind_front(&EntityResourceSignaturePass::OnExecute, this));
        }

    private:
        void OnBuild(Rg::Resolver& resolver)
        {
            if (!m_Srb)
            {
                auto renderDevice = resolver.GetDevice()->GetRenderDevice();

                Ptr resourceSignature = CreateResourceSignature(renderDevice);
                resourceSignature->CreateShaderResourceBinding(&m_Srb);
            }

            resolver.SetUserData(Ty::RGSignature(), m_Srb);
        }

        void OnExecute(const Rg::ResourceStorage& storage, Dg::IDeviceContext*)
        {
            auto transforms      = storage.GetResource(c_RGTransformTable)->AsBuffer();
            auto renderInstances = storage.GetResource(c_RGRenderInstanceTable)->AsBuffer();
            auto lightInstances  = storage.GetResource(c_RGLightInstanceTable)->AsBuffer();

            Rhi::BindAllInSrb(
                m_Srb, ShaderFlags, "AllTransforms", transforms->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
            Rhi::BindAllInSrb(m_Srb, ShaderFlags, "AllRenderInstances",
                              renderInstances->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
            Rhi::BindAllInSrb(m_Srb, ShaderFlags, "AllLightInstances",
                              lightInstances->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
        }

    private:
        [[nodiscard]] static Ptr<Dg::IPipelineResourceSignature> CreateResourceSignature(
            Dg::IRenderDevice* renderDevice)
        {
            constexpr std::array resources{
                Dg::PipelineResourceDesc{ ShaderFlags, "AllTransforms", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ ShaderFlags, "AllRenderInstances", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
                Dg::PipelineResourceDesc{ ShaderFlags, "AllLightInstances", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV,
                                          Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC },
            };

            Dg::PipelineResourceSignatureDesc desc{ .Resources    = resources.data(),
                                                    .NumResources = Rhi::Count32(resources),
                                                    .BindingIndex = 2 };

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
        EntityResourceSignature_GraphicsPass()
        {
            Name("Initialize Entity Resource Signature (Graphics)");
        }

        [[nodiscard]] static const auto& RGSignature() noexcept
        {
            return c_RGEntityResourceSignature_Graphics;
        }
    };

    class EntityResourceSignature_ComputePass final
        : public EntityResourceSignaturePass<EntityResourceSignature_ComputePass, Dg::SHADER_TYPE_COMPUTE>
    {
    public:
        EntityResourceSignature_ComputePass()
        {
            Name("Initialize Entity Resource Signature (Compute)");
        }

        [[nodiscard]] static const auto& RGSignature() noexcept
        {
            return c_RGEntityResourceSignature_Compute;
        }
    };
} // namespace Ame::Gfx