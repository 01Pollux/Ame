#pragma once

#include <Ecs/World.hpp>
#include <Rhi/Core.hpp>
#include <Shading/Technique.hpp>
#include <Graphics/RenderGraph/Common/EntityIterator.hpp>

namespace Ame::Gfx
{
    inline void StandardRenderObjects(
        Ecs::World&                 world,
        Dg::IShaderResourceBinding* ersSrb,
        Dg::IDeviceContext*         deviceContext,
        Rhi::MaterialTechnique*     technique)
    {
        auto commandsIterator = world->get<EntityDrawCommandsCategoryIterator>();

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

                auto pso = technique->GetPipelineState(renderableDesc.Vertices.Desc, renderableDesc.Material);
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