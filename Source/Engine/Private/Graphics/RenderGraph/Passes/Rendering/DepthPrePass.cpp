#include <Graphics/RenderGraph/Passes/Rendering/DepthPrePass.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityResourceSignature.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/EntityEmptyVertexBuffers.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/StdRenderObjects.hpp>

#include <Shaders/Rendering/DepthPrepass.hpp>

namespace Ame::Gfx
{
    DepthPrePass::DepthPrePass(Ecs::World* world) : m_World(world)
    {
        Name("Depth Prepass")
            .Build(std::bind_front(&DepthPrePass::OnBuild, this))
            .Execute(std::bind_front(&DepthPrePass::OnExecute, this));
    }

    //

    void DepthPrePass::BindResourcesOnce(const Rg::ResourceStorage& storage)
    {
        if (m_PassData)
        {
            return;
        }

        m_PassData.Srbs[0] = storage.GetUserData<Dg::IShaderResourceBinding>(
            c_RGFrameDataResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
        m_PassData.Srbs[1] = storage.GetUserData<Dg::IShaderResourceBinding>(
            c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);
    }

    void DepthPrePass::CreateResourcesOnce(const Rg::ResourceStorage& storage)
    {
        if (m_Technique)
        {
            return;
        }

        Rhi::MaterialRenderState renderState{
            .Name = "Forward+::DepthPrePass",
            .Links{ .Sources{ { Dg::SHADER_TYPE_VERTEX, Rhi::DepthPrepass_VertexShader().GetCreateInfo() } },
                    .ActiveShaders = Dg::SHADER_TYPE_ALL_GRAPHICS & ~Dg::SHADER_TYPE_PIXEL },
            .Signatures = m_PassData.Srbs |
                          std::views::transform([](auto srb) { return Ptr(srb->GetPipelineResourceSignature()); }) |
                          std::ranges::to<std::vector>(),
            .DSFormat = DepthPrePass::DepthFormat,
        };

        auto renderDevice = storage.GetDevice()->GetRenderDevice();
        m_Technique       = AmeCreate(Rhi::MaterialTechnique, renderDevice, std::move(renderState));
    }

    //

    void DepthPrePass::OnBuild(Rg::Resolver& resolver)
    {
        auto textureDesc      = resolver.GetBackbufferDesc();
        textureDesc.Format    = DepthPrePass::DepthFormat;
        textureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_DEPTH_STENCIL;

        resolver.CreateTexture(c_RGDepthImage, nullptr, textureDesc);
        m_PassData.DepthView =
            resolver.WriteTexture(c_RGDepthImage, Dg::BIND_DEPTH_STENCIL, Dg::TEXTURE_VIEW_DEPTH_STENCIL);

        resolver.ReadUserData(c_RGFrameDataResourceSignature_Graphics);
        resolver.ReadUserData(c_RGEntityResourceSignature_Graphics);
        resolver.ReadUserData(c_RGLightingResourceSignature_Graphics);
    }

    void DepthPrePass::OnExecute(const Rg::ResourceStorage& storage, Dg::IDeviceContext* deviceContext)
    {
        auto dsv = storage.GetTextureView(m_PassData.DepthView);

        deviceContext->SetRenderTargets(0, nullptr, dsv, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        deviceContext->ClearDepthStencil(
            dsv, Dg::CLEAR_DEPTH_FLAG, 1.f, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //

        BindResourcesOnce(storage);
        CreateResourcesOnce(storage);

        StdRenderObjectDesc drawProps{
            m_World, m_PassData.Srbs, nullptr, deviceContext, m_Technique,
        };

        StandardRenderObjects(drawProps);
    }
} // namespace Ame::Gfx