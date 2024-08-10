#include <Graphics/RenderGraph/Passes/RenderIndirectCommands.hpp>

#include <Shaders/RenderIndirectCommandsShader.hpp>
#include <Rhi/Utils/DeviceWithCache.hpp>
#include <Rhi/Utils/SRBBinder.hpp>

#include <Ecs/World.hpp>
#include <Graphics/RenderGraph/Common/EntityIterator.hpp>

namespace Ame::Gfx
{
    RenderIndirectCommandsPass::RenderIndirectCommandsPass(
        Ecs::World* world) :
        m_World(world)
    {
        Name("Render Indirect Commands Pass")
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&RenderIndirectCommandsPass::Build, this))
            .Execute(std::bind_front(&RenderIndirectCommandsPass::Execute, this));
    }

    //

    void RenderIndirectCommandsPass::CreateResources(
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

    void RenderIndirectCommandsPass::Build(
        RG::Resolver& resolver)
    {
        resolver.ReadBuffer(RGRenderInstances, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);
        resolver.ReadBuffer(RGSortedRenderInstances, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);

        resolver.ReadBuffer(RGDrawCommands, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);
        resolver.ReadBuffer(RGDrawCommandCounts, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);

        resolver.WriteTexture(RGRenderTarget, Dg::BIND_RENDER_TARGET, Dg::TEXTURE_VIEW_RENDER_TARGET);
        auto rtvFormat = resolver.GetTextureDesc(RGRenderTarget.GetResource())->Format;

        //

        auto renderDevice = resolver.GetDevice()->GetRenderDevice();
        CreateResources(renderDevice, rtvFormat);
    }

    void RenderIndirectCommandsPass::Execute(
        const RG::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        auto frameData = storage.GetResource(c_RGFrameData)->AsBuffer();

        CRef renderInstances       = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGRenderInstances));
        CRef sortedRenderInstances = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGSortedRenderInstances));

        auto drawCommands      = storage.GetResource(c_RGDrawCommands)->AsBuffer();
        auto drawCommandCounts = storage.GetResource(c_RGDrawCommandCounts)->AsBuffer();

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
                auto pso = m_Technique->GetPipelineState(Dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, renderableDesc.Material, &srb);
                deviceContext->SetPipelineState(pso);

                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "FrameDataBuffer", frameData->Resource);

                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "RenderInstances", renderInstances.get().View);
                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "SortedRenderInstances", sortedRenderInstances.get().View);

                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "PositionBuffer", renderableVertices.Position.Buffer->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "NormalBuffer", renderableVertices.Normal.Buffer->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "TexCoordBuffer", renderableVertices.TexCoord.Buffer->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
                Rhi::BindAllInSrb(srb, Dg::SHADER_TYPE_ALL_GRAPHICS, "TangentBuffer", renderableVertices.Tangent.Buffer->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));

                deviceContext->CommitShaderResources(srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

                Dg::DrawIndexedIndirectAttribs drawAttribs{
                    renderableDesc.Indices.Type,
                    drawCommands->Resource,
                    Dg::DRAW_FLAG_VERIFY_ALL | Dg::DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT,
                    25, // TODO: Better count
                    row.GetDrawArgOffset(),
                    sizeof(Rhi::DrawIndexedIndirectCommand),
                    Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                    drawCommandCounts->Resource,
                    row.GetCounterOffset(),
                    Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION
                };
                deviceContext->SetIndexBuffer(renderableDesc.Indices.Buffer, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                deviceContext->DrawIndexedIndirect(drawAttribs);
            }
        }
    }
} // namespace Ame::Gfx