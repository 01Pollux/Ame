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
        auto rtvFormat    = storage.GetResource(c_RGFinalImage)->AsTexture()->Desc.Format;
        auto dsvFormat    = storage.GetResource(c_RGDepthImage)->AsTexture()->Desc.Format;

        Rhi::MaterialRenderState renderState{
            .Name         = "Forward+::RenderObjects",
            .DepthStencil = Dg::DSS_EnableDepthNoWrites,
            .Links{
                .Sources{
                    { Dg::SHADER_TYPE_VERTEX, Rhi::ForwardPlus_RenderObjects_VertexShader().GetCreateInfo() },
                    { Dg::SHADER_TYPE_PIXEL, Rhi::ForwardPlus_RenderObjects_PixelShader().GetCreateInfo() } } },
            .Signatures    = { Ptr(srb->GetPipelineResourceSignature()) },
            .RenderTargets = { rtvFormat },
            .DSFormat      = dsvFormat
        };
        renderState.DepthStencil.DepthFunc = Dg::COMPARISON_FUNC_LESS_EQUAL;

        m_Technique = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    void ForwardPlus_RenderObjectsPass::OnBuild(
        Rg::Resolver& resolver)
    {
        resolver.ReadTexture(c_RGDepthImage("ForwardPlus_Depth"), Dg::BIND_DEPTH_STENCIL, Dg::TEXTURE_VIEW_DEPTH_STENCIL);
        resolver.WriteTexture(c_RGFinalImage("ForwardPlus_RenderObjects"), Dg::BIND_RENDER_TARGET, Dg::TEXTURE_VIEW_RENDER_TARGET);

        resolver.ReadUserData(c_RGEntityResourceSignature_Graphics);
        resolver.ReadUserData(c_RGEntityEmptyVertexBuffers);
    }

    void ForwardPlus_RenderObjectsPass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        auto ersSrb = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);

        TryCreateResources(storage, ersSrb);
        StandardRenderObjects(*m_World, ersSrb, deviceContext, m_Technique);
    }
} // namespace Ame::Gfx