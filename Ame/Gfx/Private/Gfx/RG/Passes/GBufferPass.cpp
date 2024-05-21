#include <Gfx/RG/Passes/GBufferPass.hpp>

#include <Gfx/Shading/Material.hpp>
#include <Gfx/Constants.hpp>

namespace Ame::Gfx::RG::Std
{
    namespace CD = Constants::DescriptorRanges;

    //

    GBufferPass::GBufferPass(
        Cache::MaterialBindingCache& MaterialCache) :
        m_MaterialCache(MaterialCache)
    {
        Name("GBufferPass")
            .SetFlags(PassFlags::Graphics)
            .Build(
                [this](Resolver& RgResolver)
                {
                    RgResolver.ReadBuffer(
                        Names::EntityCommandCounter("GBufferPass"),
                        Rhi::ShaderType::GRAPHICS_SHADERS,
                        Rhi::ResourceFormat::R32_UINT);
                    RgResolver.ReadBuffer(
                        Names::EntityCommandBuffer("GBufferPass"),
                        Rhi::ShaderType::GRAPHICS_SHADERS,
                        Rhi::ResourceFormat::R32_UINT);
                })
            .Execute(
                [this](const ResourceStorage& RgStorage, Rhi::CommandList* CommandList)
                {
                    auto& TransformsTable      = RgStorage.GetResourceViewHandle(Names::TransformsTable("CollectPass"));
                    auto& RenderInstancesTable = RgStorage.GetResourceViewHandle(Names::RenderInstancesTable("CollectPass"));

                    auto& CommandsBuffer = RgStorage.GetResource(Names::EntityCommandBuffer);
                    auto& CounterBuffer  = RgStorage.GetResource(Names::EntityCommandCounter);

                    //

                    auto FrameDataSet  = CommandList->AllocateSet(CD::FrameData_SetIndex);
                    auto EntityDataSet = CommandList->AllocateSet(CD::EntityData_SetIndex);

                    nri::Descriptor* FrameDescriptors[]{
                        RgStorage.GetFrameResourceHandle().Unwrap()
                    };

                    nri::Descriptor* EntityDescriptors[]{
                        TransformsTable.Unwrap(),
                        RenderInstancesTable.Unwrap()
                    };

                    FrameDataSet.SetRange(0, { FrameDescriptors, Rhi::Count32(FrameDescriptors) });
                    EntityDataSet.SetRange(0, { EntityDescriptors, Rhi::Count32(EntityDescriptors) });

                    //
                    return;

                    auto EntStore = RgStorage.GetEntityStore();
                    for (auto& Row : EntStore.GetCountedRows())
                    {
                        m_MaterialCache.get().Bind(*CommandList, *Row->Material);

                        CommandList->SetDescriptorSet(CD::FrameData_SetIndex, FrameDataSet);
                        CommandList->SetDescriptorSet(CD::EntityData_SetIndex, EntityDataSet);

                        auto PipelineState = Row->Material->GetPipelineState(
                                                              { GBufferFormats,
                                                                DepthFormat })
                                                 .get();
                        CommandList->SetPipelineState(PipelineState);

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