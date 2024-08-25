#pragma once

#include <Ecs/World.hpp>
#include <Rhi/Core.hpp>
#include <Shading/Technique.hpp>
#include <Graphics/RenderGraph/Common/EntityIterator.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/LightingResourceSignature.hpp>

namespace Ame::Gfx
{
    using StdLightDrawPropCategories = std::array<LightingDrawProp, std::to_underlying(DrawInstanceType::Count)>;
    struct StdRenderObjectDesc
    {
        Ecs::World*                            World;
        std::span<Dg::IShaderResourceBinding*> Srbs;
        Dg::IShaderResourceBinding*            LightSrb = nullptr;
        Dg::IDeviceContext*                    DeviceContext;
        Rhi::MaterialTechnique*                Technique;
        StdLightDrawPropCategories             LightDrawProps;
    };

    inline void StandardRenderObjects(
        const StdRenderObjectDesc& desc)
    {
        auto commandsIterator = (*desc.World)->get<EntityDrawCommandsCategoryIterator>();

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

                auto pso = desc.Technique->GetPipelineState(renderableDesc.Vertices.Desc, renderableDesc.Material);
                if (cachePso != pso)
                {
                    cachePso = pso;
                    desc.DeviceContext->SetPipelineState(pso);
                }

                auto matSrb = renderableDesc.Material->GetBindings();
                if (cacheMaterialSrb != matSrb)
                {
                    cacheMaterialSrb = matSrb;
                    for (auto srb : desc.Srbs)
                    {
                        desc.DeviceContext->CommitShaderResources(srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                    }
                    if (desc.LightSrb)
                    {
                        LightingResourceSignaturePass_GraphicsPass::Bind(desc.LightSrb, desc.LightDrawProps[std::to_underlying(group.GetType())]);
                        desc.DeviceContext->CommitShaderResources(desc.LightSrb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                    }
                    desc.DeviceContext->CommitShaderResources(matSrb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                }

                desc.DeviceContext->SetVertexBuffers(0, Rhi::Count32(vertexBuffers), vertexBuffers, nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Dg::SET_VERTEX_BUFFERS_FLAG_RESET);
                desc.DeviceContext->SetIndexBuffer(renderableDesc.Indices.Buffer, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                desc.DeviceContext->DrawIndexed(drawAttribs);
            }
        }
    }
} // namespace Ame::Gfx