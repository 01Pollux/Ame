#include <Graphics/RenderGraph/Passes/ForwardPlus_RenderObjects.hpp>

#include <Shaders/RenderIndirectCommandsShader.hpp>
#include <Rhi/Utils/DeviceWithCache.hpp>
#include <Rhi/Utils/SRBBinder.hpp>

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
            .Build(std::bind_front(&ForwardPlus_RenderObjects::Build, this))
            .Execute(std::bind_front(&ForwardPlus_RenderObjects::Execute, this));
    }

    //

    void ForwardPlus_RenderObjects::CreateResources(
        Dg::IRenderDevice* renderDevice,
        Dg::TEXTURE_FORMAT rtvFormat)
    {
        Rhi::MaterialRenderState renderState;
        renderState.RenderTargets = { rtvFormat };

        Rhi::RenderIndirectCommandsVertexShader vertexShader;
        Rhi::RenderIndirectCommandsPixelShader  pixelShader;

        renderState.Links.ShaderSources.emplace(Dg::SHADER_TYPE_VERTEX, vertexShader.GetCreateInfo());
        renderState.Links.ShaderSources.emplace(Dg::SHADER_TYPE_PIXEL, pixelShader.GetCreateInfo());

        m_Technique = Rhi::MaterialTechnique::Create(renderDevice, std::move(renderState));
    }

    //

    void ForwardPlus_RenderObjects::Build(
        RG::Resolver& resolver)
    {
        resolver.ReadBuffer(RGRenderInstances, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);
        resolver.ReadBuffer(RGSortedRenderInstances, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);

        resolver.WriteTexture(RGRenderTarget, Dg::BIND_RENDER_TARGET, Dg::TEXTURE_VIEW_RENDER_TARGET);
        auto rtvFormat = resolver.GetTextureDesc(RGRenderTarget.GetResource())->Format;

        //

        auto renderDevice = resolver.GetDevice()->GetRenderDevice();
        CreateResources(renderDevice, rtvFormat);
    }

    void ForwardPlus_RenderObjects::Execute(
        const RG::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        auto frameData = storage.GetResource(c_RGFrameData)->AsBuffer();

        CRef renderInstances       = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGRenderInstances));
        CRef sortedRenderInstances = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGSortedRenderInstances));

        //

        auto& world            = *m_World;
        auto  commandsIterator = world->get<EntityDrawCommandsCategoryIterator>();

        Dg::IShaderResourceBinding* srb = nullptr;
        for (auto& group : commandsIterator->GetGroups())
        {
            for (auto& row : group.GetRows())
            {
                auto& renderable         = row.Command.get().Renderable;
                auto& renderableDesc     = renderable->GetRenderableDesc();
                auto& renderableVertices = renderableDesc.Vertices;

                // TODO: Configure topoly
                auto pso = m_Technique->GetPipelineState(renderableDesc.Vertices.Desc, renderableDesc.Material, &srb);
                deviceContext->SetPipelineState(pso);

                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "FrameDataBuffer", frameData->Resource);
                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "RenderInstances", renderInstances.get().View);
                deviceContext->CommitShaderResources(srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

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

                deviceContext->SetVertexBuffers(0, Rhi::Count32(vertexBuffers), vertexBuffers, nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Dg::SET_VERTEX_BUFFERS_FLAG_RESET);
                deviceContext->SetIndexBuffer(renderableDesc.Indices.Buffer, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                deviceContext->DrawIndexed(drawAttribs);
            }
        }
    }
} // namespace Ame::Gfx