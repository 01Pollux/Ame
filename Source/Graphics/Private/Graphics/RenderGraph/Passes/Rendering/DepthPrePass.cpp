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
        const Rg::ResourceStorage& storage)
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
            .Signatures = m_PassData.Srbs |
                          std::views::transform([](auto srb)
                                                { return Ptr(srb->GetPipelineResourceSignature()); }) |
                          std::ranges::to<std::vector>(),
            .DSFormat = DepthPrePass::DepthFormat,
        };

        auto renderDevice = storage.GetDevice()->GetRenderDevice();
        m_Technique       = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    void DepthPrePass::OnBuild(
        Rg::Resolver& resolver)
    {
        auto textureDesc      = resolver.GetBackbufferDesc();
        textureDesc.Format    = DepthPrePass::DepthFormat;
        textureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_DEPTH_STENCIL;

        resolver.CreateTexture(c_RGDepthImage, nullptr, textureDesc);
        m_PassData.DepthView = resolver.WriteTexture(c_RGDepthImage, Dg::TEXTURE_VIEW_DEPTH_STENCIL);

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

    void DepthPrePass::OnExecute(
        const Rg::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        deviceContext->SetRenderTargets(0, nullptr, m_PassData.DepthView, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        deviceContext->ClearDepthStencil(m_PassData.DepthView, Dg::CLEAR_DEPTH_FLAG, 1.f, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

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