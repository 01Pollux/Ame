#include <Graphics/RenderGraph/Passes/ForwardPlus_RenderObjects.hpp>
#include <Graphics/RenderGraph/Passes/Initialize_EntityResourceSignature.hpp>

#include <Shaders/RenderIndirectCommandsShader.hpp>
#include <Rhi/Utils/DeviceWithCache.hpp>

#include <Ecs/World.hpp>
#include <Graphics/RenderGraph/Common/EntityIterator.hpp>

namespace Ame::Gfx
{
    ForwardPlus_RenderObjects::ForwardPlus_RenderObjects(
        Ecs::World* world) :
        m_World(world)
    {
        Name("Render Objects")
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&ForwardPlus_RenderObjects::OnBuild, this))
            .Execute(std::bind_front(&ForwardPlus_RenderObjects::OnExecute, this));
    }

    //

    void ForwardPlus_RenderObjects::TryCreateResources(
        const RG::ResourceStorage&  storage,
        Dg::IShaderResourceBinding* srb)
    {
        if (m_Technique)
        {
            return;
        }

        auto renderDevice = storage.GetDevice()->GetRenderDevice();
        auto rtvFormat    = storage.GetResource(RGRenderTarget.GetResource())->AsTexture()->Desc.Format;

        Rhi::MaterialRenderState renderState{
            .Name          = "Forward+::RenderObjects",
            .RenderTargets = { rtvFormat }
        };

        Rhi::RenderIndirectCommandsVertexShader vertexShader;
        Rhi::RenderIndirectCommandsPixelShader  pixelShader;

        renderState.Links.ShaderSources.emplace(Dg::SHADER_TYPE_VERTEX, vertexShader.GetCreateInfo());
        renderState.Links.ShaderSources.emplace(Dg::SHADER_TYPE_PIXEL, pixelShader.GetCreateInfo());

        renderState.Signatures.emplace_back(srb->GetPipelineResourceSignature());

        m_Technique = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    void ForwardPlus_RenderObjects::OnBuild(
        RG::Resolver& resolver)
    {
        resolver.ReadUserData(Initialize_EntityResourceSignature_Graphics::RGEntityResourceSignature);
        resolver.WriteTexture(RGRenderTarget, Dg::BIND_RENDER_TARGET, Dg::TEXTURE_VIEW_RENDER_TARGET);
    }

    void ForwardPlus_RenderObjects::OnExecute(
        const RG::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        auto ersSrb = storage.GetUserData<Dg::IShaderResourceBinding>(Initialize_EntityResourceSignature_Graphics::RGEntityResourceSignature, Dg::IID_ShaderResourceBinding);

        auto& world            = *m_World;
        auto  commandsIterator = world->get<EntityDrawCommandsCategoryIterator>();

        //

        TryCreateResources(storage, ersSrb);

        //

        Dg::IShaderResourceBinding* cacheMaterialSrb = nullptr;
        Dg::IPipelineState*         cachePso         = nullptr;
        for (auto& group : commandsIterator->GetGroups())
        {
            for (auto& row : group.GetRows())
            {
                auto& renderable         = row.Command.get().Renderable;
                auto& renderableDesc     = renderable->GetRenderableDesc();
                auto& renderableVertices = renderableDesc.Vertices;

                //

                Dg::DrawIndexedAttribs drawAttribs{
                    renderableDesc.Indices.Count,
                    renderableDesc.Indices.Type,
                    Dg::DRAW_FLAG_VERIFY_ALL | Dg::DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT,
                    1,
                    renderableDesc.Indices.Offset,
                    renderableDesc.Vertices.Offset,
                    row.InstanceOffset
                };

                Dg::IBuffer* vertexBuffers[]{
                    renderableVertices.Position.Buffer,
                    renderableVertices.Normal.Buffer,
                    renderableVertices.TexCoord.Buffer,
                    renderableVertices.Tangent.Buffer
                };

                //

                // TODO: Configure topology
                auto pso = m_Technique->GetPipelineState(renderableDesc.Vertices.Desc, renderableDesc.Material);
                if (cachePso != pso)
                {
                    cachePso = pso;
                    deviceContext->SetPipelineState(pso);
                }

                auto matSrb = renderableDesc.Material->GetBindings();
                if (cacheMaterialSrb != matSrb)
                {
                    cacheMaterialSrb = matSrb;
                    deviceContext->CommitShaderResources(ersSrb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                    deviceContext->CommitShaderResources(matSrb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                }

                deviceContext->SetVertexBuffers(0, Rhi::Count32(vertexBuffers), vertexBuffers, nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Dg::SET_VERTEX_BUFFERS_FLAG_RESET);
                deviceContext->SetIndexBuffer(renderableDesc.Indices.Buffer, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                deviceContext->DrawIndexed(drawAttribs);
            }
        }
    }
} // namespace Ame::Gfx