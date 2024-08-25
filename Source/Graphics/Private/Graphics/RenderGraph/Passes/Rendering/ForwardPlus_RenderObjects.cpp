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
        const Rg::ResourceStorage& storage)
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
            .Signatures = m_PassData.Srbs |
                          std::views::transform([](auto srb)
                                                { return Ptr(srb->GetPipelineResourceSignature()); }) |
                          std::ranges::to<std::vector>(),
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

        m_PassData.Srbs[0]  = resolver.ReadUserData<Dg::IShaderResourceBinding>(c_RGFrameDataResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
        m_PassData.Srbs[1]  = resolver.ReadUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
        m_PassData.LightSrb = resolver.ReadUserData<Dg::IShaderResourceBinding>(c_RGLightingResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);

        m_PassData.LightDrawProps[std::to_underlying(DrawInstanceType::Opaque)] = {
            resolver.ReadBuffer(c_RGLightIndices_Opaque, Dg::BUFFER_VIEW_SHADER_RESOURCE),
            resolver.ReadTexture(c_RGLightHeads_Opaque, Dg::TEXTURE_VIEW_SHADER_RESOURCE)
        };
        m_PassData.LightDrawProps[std::to_underlying(DrawInstanceType::Transparent)] = {
            resolver.ReadBuffer(c_RGLightIndices_Transparent, Dg::BUFFER_VIEW_SHADER_RESOURCE),
            resolver.ReadTexture(c_RGLightHeads_Transparent, Dg::TEXTURE_VIEW_SHADER_RESOURCE)
        };
    }

    void ForwardPlus_RenderObjectsPass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        deviceContext->SetRenderTargets(1, &m_PassData.RenderTarget, m_PassData.DepthView, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //

        TryCreateResources(storage);

        StdRenderObjectDesc drawProps{
            m_World,
            m_PassData.Srbs,
            m_PassData.LightSrb,
            deviceContext,
            m_Technique,
            m_PassData.LightDrawProps
        };
        StandardRenderObjects(drawProps);
    }
} // namespace Ame::Gfx