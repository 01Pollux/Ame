#include <Graphics/RenderGraph/Passes/Rendering/DepthPrePass.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityEmptyVertexBuffers.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/StdRenderObjects.hpp>

#include <Shaders/Rendering/DepthPrepass.hpp>

namespace Ame::Gfx
{
    DepthPrePass::DepthPrePass(
        Ecs::World* world) :
        m_World(world)
    {
        Name("Depth Prepass")
            .Flags(Rg::PassFlags::Graphics)
            .Build(std::bind_front(&DepthPrePass::OnBuild, this))
            .Execute(std::bind_front(&DepthPrePass::OnExecute, this));
    }

    //

    void DepthPrePass::TryCreateResources(
        const Rg::ResourceStorage&  storage,
        Dg::IShaderResourceBinding* srb)
    {
        if (m_Technique)
        {
            return;
        }

        Rhi::MaterialRenderState renderState{
            .Name = "Forward+::DepthPrePass",
            .Links{
                .Sources{ { Dg::SHADER_TYPE_VERTEX, Rhi::DepthPrepass_VertexShader().GetCreateInfo() } },
                .ActiveShaders = Dg::SHADER_TYPE_ALL_GRAPHICS & ~Dg::SHADER_TYPE_PIXEL },
            .Signatures = { Ptr(srb->GetPipelineResourceSignature()) },
            .DSFormat   = DepthPrePass::DepthFormat,
        };

        auto renderDevice = storage.GetDevice()->GetRenderDevice();
        m_Technique       = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    Co::result<void> DepthPrePass::OnBuild(
        Rg::Resolver& resolver)
    {
        auto textureDesc      = resolver.GetBackbufferDesc();
        textureDesc.Format    = DepthPrePass::DepthFormat;
        textureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_DEPTH_STENCIL;

        resolver.CreateTexture(c_RGDepthImage, nullptr, textureDesc);
        m_PassData.DepthView = co_await resolver.WriteTexture(c_RGDepthImage, Dg::TEXTURE_VIEW_DEPTH_STENCIL);

        co_await resolver.ReadUserData(c_RGEntityResourceSignature_Graphics);
    }

    void DepthPrePass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        auto ersSrb = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);

        //

        deviceContext->SetRenderTargets(0, nullptr, m_PassData.DepthView, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        deviceContext->ClearDepthStencil(m_PassData.DepthView, Dg::CLEAR_DEPTH_FLAG, 1.f, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //

        TryCreateResources(storage, ersSrb);
        StandardRenderObjects(*m_World, ersSrb, deviceContext, m_Technique);
    }
} // namespace Ame::Gfx