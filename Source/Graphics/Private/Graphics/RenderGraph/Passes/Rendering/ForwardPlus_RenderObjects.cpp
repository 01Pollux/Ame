#include <Graphics/RenderGraph/Passes/Rendering/ForwardPlus_RenderObjects.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityEmptyVertexBuffers.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/StdRenderObjects.hpp>

#include <Shaders/Rendering/ForwardPlus_RenderObjects.hpp>

namespace Ame::Gfx
{
    ForwardPlus_RenderObjectsPass::ForwardPlus_RenderObjectsPass(
        Ecs::World* world) :
        m_World(world)
    {
        Name("Render Objects")
            .Flags(Rg::PassFlags::Graphics)
            .Build(std::bind_front(&ForwardPlus_RenderObjectsPass::OnBuild, this))
            .Execute(std::bind_front(&ForwardPlus_RenderObjectsPass::OnExecute, this));
    }

    //

    void ForwardPlus_RenderObjectsPass::TryCreateResources(
        const Rg::ResourceStorage&  storage,
        Dg::IShaderResourceBinding* srb)
    {
        if (m_Technique)
        {
            return;
        }

        auto renderDevice = storage.GetDevice()->GetRenderDevice();
        auto rtvFormat    = storage.GetResource(c_RGFinalImage)->AsTexture()->GetDesc().Format;
        auto dsvFormat    = storage.GetResource(c_RGDepthImage)->AsTexture()->GetDesc().Format;

        Rhi::MaterialRenderState renderState{
            .Name         = "Forward+::RenderObjects",
            .DepthStencil = Dg::DSS_EnableDepthNoWrites,
            .Links{
                .Sources{
                    { Dg::SHADER_TYPE_VERTEX, Rhi::ForwardPlus_RenderObjects_VertexShader().GetCreateInfo() },
                    { Dg::SHADER_TYPE_PIXEL, Rhi::ForwardPlus_RenderObjects_PixelShader().GetCreateInfo() } } },
            .Signatures    = { Ptr(srb->GetPipelineResourceSignature()) },
            .RenderTargets = { rtvFormat },
            .DSFormat      = dsvFormat,
        };
        renderState.DepthStencil.DepthFunc = Dg::COMPARISON_FUNC_LESS_EQUAL;

        m_Technique = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    void ForwardPlus_RenderObjectsPass::OnBuild(
        Rg::Resolver& resolver)
    {
        m_PassData.DepthView    = resolver.ReadTexture(c_RGDepthImage, Dg::TEXTURE_VIEW_DEPTH_STENCIL);
        m_PassData.RenderTarget = resolver.WriteTexture(c_RGFinalImage, Dg::TEXTURE_VIEW_RENDER_TARGET);

        resolver.ReadUserData(c_RGEntityResourceSignature_Graphics);
    }

    void ForwardPlus_RenderObjectsPass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        auto ersSrb = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);

        //

        deviceContext->SetRenderTargets(1, &m_PassData.RenderTarget, m_PassData.DepthView, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //

        TryCreateResources(storage, ersSrb);
        StandardRenderObjects(*m_World, ersSrb, deviceContext, m_Technique);
    }
} // namespace Ame::Gfx