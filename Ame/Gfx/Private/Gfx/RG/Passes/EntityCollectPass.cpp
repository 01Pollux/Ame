#include <Gfx/RG/Passes/EntityCollectPass.hpp>

#include <Gfx/Cache/PipelineStateCache.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG::Std
{
    EntityCollectPass::EntityCollectPass(
        Ecs::Universe&             Universe,
        Cache::PipelineStateCache& PipelineStateCache) :
        m_Universe(Universe),
        m_PipelineStateCache(PipelineStateCache)
    {
        m_PipelineStateCache.get().Load(Cache::PipelineStateCache::Type::EntityCollectPass);

        Name("EntityCollectPass")
            .SetFlags(PassFlags::Compute)
            .Build(
                [this](Resolver& RgResolver)
                {
                    auto& World       = *m_Universe.get().GetActiveWorld();
                    auto  EntityCount = std::max(World.CreateFilter<const Ecs::Component::BaseRenderable>().build().count(), MinEntities);

                    RgResolver.CreateBuffer(
                        Names::EntityCommandBuffer,
                        Rhi::BufferDesc{ Math::AlignUp(sizeof(DispatchDesc) * EntityCount, BufferAlignment) });
                    RgResolver.CreateBuffer(
                        Names::EntityCommandCounter,
                        Rhi::BufferDesc{ Math::AlignUp(sizeof(int) * EntityCount, BufferAlignment) });

                    //

                    RgResolver.ReadBuffer(
                        Names::TransformsTable("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER);

                    RgResolver.ReadBuffer(
                        Names::RenderInstancesTable("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER);

                    RgResolver.WriteBuffer(
                        Names::EntityCommandCounter("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER,
                        Rhi::ResourceFormat::R32_UINT);
                    RgResolver.WriteBuffer(
                        Names::EntityCommandBuffer("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER,
                        Rhi::ResourceFormat::R32_UINT);
                })
            .Execute(
                [this](const ResourceStorage& RgStorage, Rhi::CommandList* CommandList)
                {
                    auto& TransformsTable      = RgStorage.GetResourceViewHandle(Names::TransformsTable("CollectPass"));
                    auto& RenderInstancesTable = RgStorage.GetResourceViewHandle(Names::RenderInstancesTable("CollectPass"));
                    auto& CommandsView         = RgStorage.GetResourceViewHandle(Names::EntityCommandBuffer("CollectPass"));
                    auto& CounterView          = RgStorage.GetResourceViewHandle(Names::EntityCommandCounter("CollectPass"));

                    auto PipelineState = m_PipelineStateCache.get()
                                             .Load(Cache::PipelineStateCache::Type::EntityCollectPass)
                                             .get();

                    CommandList->SetPipelineLayout(PipelineState->GetLayout());

                    //

                    auto FrameDataSet   = CommandList->AllocateSet(0);
                    auto EntityDataSet  = CommandList->AllocateSet(1);
                    auto CommandInfoSet = CommandList->AllocateSet(2);

                    nri::Descriptor* FrameDescriptors[]{
                        RgStorage.GetFrameResourceHandle().Unwrap()
                    };

                    nri::Descriptor* EntityDescriptors[]{
                        TransformsTable.Unwrap(),
                        RenderInstancesTable.Unwrap()
                    };

                    nri::Descriptor* CommandInfoDescriptors[]{
                        CommandsView.Unwrap(),
                        CounterView.Unwrap()
                    };

                    FrameDataSet.SetRange(0, { FrameDescriptors, Rhi::Count32(FrameDescriptors) });
                    EntityDataSet.SetRange(0, { EntityDescriptors, Rhi::Count32(EntityDescriptors) });
                    CommandInfoSet.SetRange(0, { CommandInfoDescriptors, Rhi::Count32(CommandInfoDescriptors) });

                    //

                    CommandList->SetDescriptorSet(0, FrameDataSet);
                    CommandList->SetDescriptorSet(1, EntityDataSet);
                    CommandList->SetDescriptorSet(2, CommandInfoSet);

                    //

                    CommandList->SetPipelineState(PipelineState);

                    auto EntStore = RgStorage.GetEntityStore();
                    for (auto& Row : EntStore.GetCountedRows())
                    {
                        DispatchDesc DispatchConstants{
                            .DrawOffset    = Row.DrawOffset,
                            .DrawCount     = Row->Count,
                            .CounterOffset = Row.CounterOffset
                        };
                        CommandList->SetConstants(0, DispatchConstants);

                        CommandList->ClearBuffer(
                            { .storageBuffer            = CounterView.Unwrap(),
                              .setIndexInPipelineLayout = 2,
                              .rangeIndex               = 1,
                              .offsetInRange            = Row.CounterOffset });

                        CommandList->Dispatch(1);
                    }
                });
    }
} // namespace Ame::Gfx::RG::Std