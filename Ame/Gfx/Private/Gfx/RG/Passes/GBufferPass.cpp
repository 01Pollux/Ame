#include <ranges>

#include <Gfx/RG/Passes/GBufferPass.hpp>
#include <Rhi/Resource/VertexView.hpp>

#include <Gfx/Shading/Material.hpp>
#include <Gfx/Constants.hpp>

namespace Ame::Gfx::RG::Std
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
            .SetFlags(PassFlags::Graphics)
            .Build(
                [this](Resolver& RgResolver)
                {
                    auto& frameData       = RgResolver.GetFrameResourceData();
                    auto  textureDesc     = RgResolver.GetBackbufferDesc();
                    textureDesc.usageMask = {};

                    for (auto [id, format] : std::views::zip(c_RenderTargetIds, c_RenderTargetFormats))
                    {
                        textureDesc.format = format;
                        RgResolver.CreateTexture(id, textureDesc);
                        RgResolver.WriteRenderTarget(id("Main"), Rhi::StageBits::DRAW, format);
                    }

                    textureDesc.format = c_DepthTargetFormat;
                    RgResolver.CreateTexture(c_DepthTargetId, textureDesc);
                    RgResolver.WriteDepthStencil(c_DepthTargetId("Main"), Rhi::StageBits::DRAW, c_DepthTargetFormat);

                    ////

                    RgResolver.ReadBuffer(
                        Names::c_TransformsTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);
                    RgResolver.ReadBuffer(
                        Names::c_RenderInstancesTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);

                    RgResolver.ReadIndirectBuffer(
                        Names::c_EntityCommandCounter("GBufferPass"));
                    RgResolver.ReadIndirectBuffer(
                        Names::c_EntityCommandBuffer("GBufferPass"));
                })
            .Execute(
                [this](const ResourceStorage& RgStorage, Rhi::CommandList* CommandList)
                {
                    auto& frameData = RgStorage.GetFrameResourceData();

                    auto& transformsTable      = RgStorage.GetResourceViewHandle(Names::c_TransformsTable("GBufferPass"));
                    auto& renderInstancesTable = RgStorage.GetResourceViewHandle(Names::c_RenderInstancesTable("GBufferPass"));

                    auto& commandsBuffer = RgStorage.GetResource(Names::c_EntityCommandBuffer);
                    auto& counterBuffer  = RgStorage.GetResource(Names::c_EntityCommandCounter);

                    //

                    nri::Descriptor* frameDescriptors[]{
                        RgStorage.GetFrameResourceHandle().Unwrap()
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

                    auto entityStore = RgStorage.GetEntityStore();

                    Rhi::DescriptorSet frameDataSet;
                    Rhi::DescriptorSet entityDataSet;

                    Rhi::Viewport viewports[3]{
                        { .width         = frameData.Viewport.x,
                          .height        = frameData.Viewport.y,
                          .depthRangeMax = 1.f }
                    };
                    Rhi::ScissorRect scissors[3]{
                        { .width  = static_cast<Rhi::Dim_t>(frameData.Viewport.x),
                          .height = static_cast<Rhi::Dim_t>(frameData.Viewport.y) }
                    };

                    for (uint32_t i = 1; i < std::size(viewports); i++)
                    {
                        viewports[i] = viewports[0];
                        scissors[i]  = scissors[0];
                    }

                    CommandList->SetViewports(viewports);
                    CommandList->SetScissorRects(scissors);

                    for (auto& row : entityStore.GetCountedRows())
                    {
                        m_MaterialCache.get().Bind(*CommandList, *row->Material);

                        if (!frameDataSet)
                        {
                            frameDataSet  = CommandList->AllocateSet(CD::c_FrameData_SetIndex);
                            entityDataSet = CommandList->AllocateSet(CD::c_EntityData_SetIndex);

                            frameDataSet.SetRange(0, { frameDescriptors, Rhi::Count32(frameDescriptors) });
                            entityDataSet.SetRange(0, { entityDescriptors, Rhi::Count32(entityDescriptors) });

                            CommandList->SetDescriptorSet(CD::c_FrameData_SetIndex, frameDataSet);
                            CommandList->SetDescriptorSet(CD::c_EntityData_SetIndex, entityDataSet);
                        }

                        auto pipelineState = row->Material->GetPipelineState(RenderState).get();
                        CommandList->SetPipelineState(pipelineState);

                        CommandList->SetVertexBuffer({ .Buffer = row->VtxBuffer });
                        CommandList->SetIndexBuffer({ .Buffer = row->IdxBuffer, .Type = row->IndexType });

                        CommandList->DrawIndirectIndexed(
                            { .DrawBuffer    = commandsBuffer.AsBuffer()->Unwrap(),
                              .DrawOffset    = row.DrawOffset,
                              .MaxDrawCount  = row->Count,
                              .CounterBuffer = counterBuffer.AsBuffer()->Unwrap(),
                              .CounterOffset = row.CounterOffset });
                    }
                });
    }
} // namespace Ame::Gfx::RG::Std