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
        Cache::CommonShader&         CommonShaders,
        Cache::MaterialBindingCache& MaterialCache) :
        m_CommonShaders(CommonShaders),
        m_MaterialCache(MaterialCache)
    {
        m_CommonShaders.get().Load(Cache::CommonShader::Type::GBufferPass_PS);

        Name("GBufferPass")
            .SetFlags(PassFlags::Graphics)
            .Build(
                [this](Resolver& RgResolver)
                {
                    auto& FrameData = RgResolver.GetFrameResourceData();
                    auto  Desc      = RgResolver.GetBackbufferDesc();
                    Desc.usageMask  = {};

                    for (auto [Id, Format] : std::views::zip(RenderTargetIds, RenderTargetFormats))
                    {
                        Desc.format = Format;
                        RgResolver.CreateTexture(Id, Desc);
                        RgResolver.WriteRenderTarget(Id("Main"), Rhi::StageBits::DRAW, Format);
                    }

                    Desc.format = DepthTargetFormat;
                    RgResolver.CreateTexture(DepthTargetId, Desc);
                    RgResolver.WriteDepthStencil(DepthTargetId("Main"), Rhi::StageBits::DRAW, DepthTargetFormat);

                    ////

                    RgResolver.ReadBuffer(
                        Names::TransformsTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);
                    RgResolver.ReadBuffer(
                        Names::RenderInstancesTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);

                    RgResolver.ReadIndirectBuffer(
                        Names::EntityCommandCounter("GBufferPass"));
                    RgResolver.ReadIndirectBuffer(
                        Names::EntityCommandBuffer("GBufferPass"));
                })
            .Execute(
                [this](const ResourceStorage& RgStorage, Rhi::CommandList* CommandList)
                {
                    auto& FrameData = RgStorage.GetFrameResourceData();

                    auto& TransformsTable      = RgStorage.GetResourceViewHandle(Names::TransformsTable("GBufferPass"));
                    auto& RenderInstancesTable = RgStorage.GetResourceViewHandle(Names::RenderInstancesTable("GBufferPass"));

                    auto& CommandsBuffer = RgStorage.GetResource(Names::EntityCommandBuffer);
                    auto& CounterBuffer  = RgStorage.GetResource(Names::EntityCommandCounter);

                    //

                    nri::Descriptor* FrameDescriptors[]{
                        RgStorage.GetFrameResourceHandle().Unwrap()
                    };

                    nri::Descriptor* EntityDescriptors[]{
                        TransformsTable.Unwrap(),
                        RenderInstancesTable.Unwrap()
                    };

                    //

                    Shading::MaterialShaderLink GBufferShaders;

                    GBufferShaders.Shaders.emplace_back(m_CommonShaders.get().Load(Cache::CommonShader::Type::GBufferPass_PS).get().Borrow());
                    GBufferShaders.CompileDesc.SetStage(Rhi::ShaderType::FRAGMENT_SHADER);

                    Shading::MaterialRenderState RenderState{
                        RenderTargetFormats,
                        DepthTargetFormat,
                        std::move(GBufferShaders)
                    };

                    auto EntStore = RgStorage.GetEntityStore();

                    Rhi::DescriptorSet FrameDataSet;
                    Rhi::DescriptorSet EntityDataSet;

                    Rhi::Viewport Viewports[3]{
                        { .width         = FrameData.Viewport.x,
                          .height        = FrameData.Viewport.y,
                          .depthRangeMax = 1.f }
                    };
                    Rhi::ScissorRect Scissors[3]{
                        { .width  = static_cast<Rhi::Dim_t>(FrameData.Viewport.x),
                          .height = static_cast<Rhi::Dim_t>(FrameData.Viewport.y) }
                    };

                    for (uint32_t i = 1; i < std::size(Viewports); i++)
                    {
                        Viewports[i] = Viewports[0];
                        Scissors[i]  = Scissors[0];
                    }

                    CommandList->SetViewports(Viewports);
                    CommandList->SetScissorRects(Scissors);

                    for (auto& Row : EntStore.GetCountedRows())
                    {
                        m_MaterialCache.get().Bind(*CommandList, *Row->Material);

                        if (!FrameDataSet)
                        {
                            FrameDataSet  = CommandList->AllocateSet(CD::FrameData_SetIndex);
                            EntityDataSet = CommandList->AllocateSet(CD::EntityData_SetIndex);

                            FrameDataSet.SetRange(0, { FrameDescriptors, Rhi::Count32(FrameDescriptors) });
                            EntityDataSet.SetRange(0, { EntityDescriptors, Rhi::Count32(EntityDescriptors) });

                            CommandList->SetDescriptorSet(CD::FrameData_SetIndex, FrameDataSet);
                            CommandList->SetDescriptorSet(CD::EntityData_SetIndex, EntityDataSet);
                        }

                        auto PipelineState = Row->Material->GetPipelineState(RenderState).get();
                        CommandList->SetPipelineState(PipelineState);

                        CommandList->SetVertexBuffer({ .Buffer = Row->VtxBuffer });
                        CommandList->SetIndexBuffer({ .Buffer = Row->IdxBuffer, .Type = Row->IndexType });

                        CommandList->DrawIndirectIndexed(
                            { .DrawBuffer    = CommandsBuffer.AsBuffer()->Unwrap(),
                              .DrawOffset    = Row.DrawOffset,
                              .MaxDrawCount  = Row->Count,
                              .CounterBuffer = CounterBuffer.AsBuffer()->Unwrap(),
                              .CounterOffset = Row.CounterOffset });
                    }
                });
    }
} // namespace Ame::Gfx::RG::Std