#include <Graphics/RenderGraph/Passes/Rendering/ForwardPlus_RenderObjects.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityEmptyVertexBuffers.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/StdRenderObjects.hpp>

#include <Shaders/Rendering/ForwardPlus_RenderObjects.hpp>

#include <boost/range/join.hpp>

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

    void ForwardPlus_RenderObjectsPass::BindResourceOnce(
        const Rg::ResourceStorage& storage)
    {
        if (m_PassData)
        {
            return;
        }

        m_PassData.Srbs[0]  = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGFrameDataResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
        m_PassData.Srbs[1]  = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
        m_PassData.LightSrb = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGLightingResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
    }

    void ForwardPlus_RenderObjectsPass::CreateResourcesOnce(
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

        renderState.Signatures.emplace_back(Ptr(m_PassData.LightSrb->GetPipelineResourceSignature()));
        renderState.DepthStencil.DepthFunc = Dg::COMPARISON_FUNC_LESS_EQUAL;

        m_Technique = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    void ForwardPlus_RenderObjectsPass::OnBuild(
        Rg::Resolver& resolver)
    {
        m_PassData.DepthView    = resolver.ReadTexture(c_RGDepthImage, Dg::BIND_DEPTH_STENCIL, Dg::TEXTURE_VIEW_DEPTH_STENCIL);
        m_PassData.RenderTarget = resolver.WriteTexture(c_RGFinalImage, Dg::BIND_RENDER_TARGET, Dg::TEXTURE_VIEW_RENDER_TARGET);

        m_PassData.LightIndices_Transparent = resolver.ReadBuffer(c_RGLightIndices_Transparent, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);
        m_PassData.LightIndices_Opaque      = resolver.ReadBuffer(c_RGLightIndices_Opaque, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);

        m_PassData.LightHeads_Transparent = resolver.ReadTexture(c_RGLightHeads_Transparent, Dg::BIND_SHADER_RESOURCE, Dg::TEXTURE_VIEW_SHADER_RESOURCE);
        m_PassData.LightHeads_Opaque      = resolver.ReadTexture(c_RGLightHeads_Opaque, Dg::BIND_SHADER_RESOURCE, Dg::TEXTURE_VIEW_SHADER_RESOURCE);

        resolver.ReadUserData(c_RGFrameDataResourceSignature_Graphics);
        resolver.ReadUserData(c_RGEntityResourceSignature_Graphics);
        resolver.ReadUserData(c_RGLightingResourceSignature_Graphics);
    }

    void ForwardPlus_RenderObjectsPass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        BindResourceOnce(storage);
        CreateResourcesOnce(storage);

        //

        auto rtv = storage.GetTextureView(m_PassData.RenderTarget);
        auto dsv = storage.GetTextureView(m_PassData.DepthView);

        deviceContext->SetRenderTargets(1, &rtv, dsv, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //

        StdRenderObjectDesc drawProps{
            m_World,
            m_PassData.Srbs,
            m_PassData.LightSrb,
            deviceContext,
            m_Technique,
        };
        drawProps.LightDrawProps[std::to_underlying(DrawInstanceType::Opaque)] = {
            storage.GetBufferView(m_PassData.LightIndices_Opaque),
            storage.GetTextureView(m_PassData.LightHeads_Opaque),
        };
        drawProps.LightDrawProps[std::to_underlying(DrawInstanceType::Transparent)] = {
            storage.GetBufferView(m_PassData.LightIndices_Transparent),
            storage.GetTextureView(m_PassData.LightHeads_Transparent),
        };

        StandardRenderObjects(drawProps);
    }
} // namespace Ame::Gfx