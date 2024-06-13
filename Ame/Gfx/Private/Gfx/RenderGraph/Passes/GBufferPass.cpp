#include <ranges>

#include <Rhi/Resource/VertexView.hpp>
#include <Gfx/RenderGraph/Passes/GBufferPass.hpp>

#include <Gfx/Constants.hpp>
#include <Gfx/Shading/Material.hpp>

namespace Ame::Gfx
{
    using namespace EnumBitOperators;
    namespace CD = Constants::DescriptorRanges;

    //

    GBufferPass::GBufferPass(
        Cache::CommonShader&         commonShaders,
        Cache::MaterialBindingCache& materialCache) :
        m_CommonShaders(commonShaders),
        m_MaterialCache(materialCache)
    {
        m_CommonShaders.get().Load(Cache::CommonShader::Type::GBufferPass_PS);

        Name("GBufferPass")
            .SetFlags(RG::PassFlags::Graphics)
            .Build(
                [this](RG::Resolver& resolver)
                {
                    auto& frameData       = resolver.GetFrameResourceData();
                    auto  textureDesc     = resolver.GetBackbufferDesc();
                    textureDesc.usageMask = {};

                    for (auto [id, format] : std::views::zip(c_RenderTargetIds, c_RenderTargetFormats))
                    {
                        textureDesc.format = format;
                        resolver.CreateTexture(id, textureDesc);
                        resolver.WriteRenderTarget(id("Main"), Rhi::StageBits::DRAW, format);
                    }

                    textureDesc.format = c_DepthTargetFormat;
                    resolver.CreateTexture(c_DepthTargetId, textureDesc);
                    resolver.WriteDepthStencil(c_DepthTargetId("Main"), Rhi::StageBits::DRAW, c_DepthTargetFormat);

                    ////

                    resolver.ReadBuffer(
                        RG::Names::c_TransformsTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);
                    resolver.ReadBuffer(
                        RG::Names::c_RenderInstancesTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);

                    resolver.ReadIndirectBuffer(
                        RG::Names::c_EntityCommandCounter("GBufferPass"));
                    resolver.ReadIndirectBuffer(
                        RG::Names::c_EntityCommandBuffer("GBufferPass"));
                })
            .Execute(
                [this](const RG::ResourceStorage& storage, Rhi::CommandList* commandList)
                {
                    auto& frameData = storage.GetFrameResourceData();

                    auto& transformsTable      = storage.GetResourceViewHandle(RG::Names::c_TransformsTable("GBufferPass"));
                    auto& renderInstancesTable = storage.GetResourceViewHandle(RG::Names::c_RenderInstancesTable("GBufferPass"));

                    auto& commandsBuffer = *storage.GetResource(RG::Names::c_EntityCommandBuffer);
                    auto& counterBuffer  = *storage.GetResource(RG::Names::c_EntityCommandCounter);

                    //

                    nri::Descriptor* frameDescriptors[]{
                        storage.GetFrameResourceHandle().Unwrap()
                    };

                    nri::Descriptor* entityDescriptors[]{
                        transformsTable.Unwrap(),
                        renderInstancesTable.Unwrap()
                    };

                    //

                    Shading::MaterialShaderLink gBufferShaders;

                    gBufferShaders.Shaders.emplace_back(m_CommonShaders.get().Load(Cache::CommonShader::Type::GBufferPass_PS).get().Borrow());
                    gBufferShaders.CompileDesc.SetStage(Rhi::ShaderType::FRAGMENT_SHADER);

                    Shading::MaterialRenderState RenderState{
                        c_RenderTargetFormats,
                        c_DepthTargetFormat,
                        std::move(gBufferShaders)
                    };

                    // auto entityStore = storage.GetEntityStore();

                    Rhi::DescriptorSet frameDataSet;
                    Rhi::DescriptorSet entityDataSet;

                    Rhi::Viewport viewports[3]{
                        { .width         = frameData.Viewport.x(),
                          .height        = frameData.Viewport.y(),
                          .depthRangeMax = 1.f }
                    };
                    Rhi::ScissorRect scissors[3]{
                        { .width  = static_cast<Rhi::Dim_t>(frameData.Viewport.x()),
                          .height = static_cast<Rhi::Dim_t>(frameData.Viewport.y()) }
                    };

                    for (uint32_t i = 1; i < std::size(viewports); i++)
                    {
                        viewports[i] = viewports[0];
                        scissors[i]  = scissors[0];
                    }

                    commandList->SetViewports(viewports);
                    commandList->SetScissorRects(scissors);

                    /* for (auto& row : entityStore.GetCountedRows())
                     {
                         m_MaterialCache.get().Bind(*commandList, *row->Material);

                         if (!frameDataSet)
                         {
                             frameDataSet  = commandList->AllocateSet(CD::c_FrameData_SetIndex);
                             entityDataSet = commandList->AllocateSet(CD::c_EntityData_SetIndex);

                             frameDataSet.SetRange(0, { frameDescriptors, Rhi::Count32(frameDescriptors) });
                             entityDataSet.SetRange(0, { entityDescriptors, Rhi::Count32(entityDescriptors) });

                             commandList->SetDescriptorSet(CD::c_FrameData_SetIndex, frameDataSet);
                             commandList->SetDescriptorSet(CD::c_EntityData_SetIndex, entityDataSet);
                         }

                         auto pipelineState = row->Material->GetPipelineState(RenderState).get();
                         commandList->SetPipelineState(pipelineState);

                         commandList->SetVertexBuffer({ .Buffer = row->VtxBuffer });
                         commandList->SetIndexBuffer({ .Buffer = row->IdxBuffer, .Type = row->IndexType });

                         commandList->DrawIndirectIndexed(
                             { .DrawBuffer    = commandsBuffer.AsBuffer()->Unwrap(),
                               .DrawOffset    = row.DrawOffset,
                               .MaxDrawCount  = row->Count,
                               .CounterBuffer = counterBuffer.AsBuffer()->Unwrap(),
                               .CounterOffset = row.CounterOffset });
                     }*/
                });
    }
} // namespace Ame::Gfx