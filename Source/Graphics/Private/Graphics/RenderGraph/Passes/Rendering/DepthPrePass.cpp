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
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&DepthPrePass::OnBuild, this))
            .Execute(std::bind_front(&DepthPrePass::OnExecute, this));
    }

    //

    void DepthPrePass::TryCreateResources(
        const RG::ResourceStorage&  storage,
        Dg::IShaderResourceBinding* srb)
    {
        if (m_Technique)
        {
            return;
        }

        auto renderDevice = storage.GetDevice()->GetRenderDevice();
        auto dsvFormat    = storage.GetResource(c_RGDepthImage)->AsTexture()->Desc.Format;

        Rhi::MaterialRenderState renderState{
            .Name     = "Forward+::DepthPrePass",
            .DSFormat = dsvFormat,
        };

        renderState.Links.Sources.emplace(Dg::SHADER_TYPE_VERTEX, Rhi::DepthPrepass_VertexShader().GetCreateInfo());
        renderState.Links.ActiveShaders = Dg::SHADER_TYPE_ALL_GRAPHICS & ~Dg::SHADER_TYPE_PIXEL;

        renderState.Signatures.emplace_back(srb->GetPipelineResourceSignature());

        m_Technique = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    void DepthPrePass::OnBuild(
        RG::Resolver& resolver)
    {
        RG::DepthStencilViewDesc dsv{
            {},
            RG::DsvCustomDesc{
                .Depth      = 1.0f,
                .ClearType  = RG::EDSClearType::Depth,
                .ForceDepth = true,
            }
        };

        auto textureDesc      = resolver.GetBackbufferDesc();
        textureDesc.Format    = Dg::TEX_FORMAT_D32_FLOAT;
        textureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE | Dg::BIND_DEPTH_STENCIL;

        resolver.CreateTexture(c_RGDepthImage, textureDesc);
        resolver.WriteTexture(c_RGDepthImage("DepthPrePass"), Dg::BIND_DEPTH_STENCIL, dsv);

        resolver.ReadUserData(c_RGEntityResourceSignature_Graphics);
        resolver.ReadUserData(c_RGEntityEmptyVertexBuffers);
    }

    void DepthPrePass::OnExecute(
        const RG::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        auto ersSrb = storage.GetUserData<Dg::IShaderResourceBinding>(c_RGEntityResourceSignature_Graphics, Dg::IID_ShaderResourceBinding);

        TryCreateResources(storage, ersSrb);
        StandardRenderObjects(*m_World, ersSrb, deviceContext, m_Technique);
    }
} // namespace Ame::Gfx