#include <Gfx/RG/Passes/EntityCollectPass.hpp>

#include <Gfx/Cache/CommonPipelineState.hpp>
#include <Gfx/Constants.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG::Std
{
    namespace CD = Constants::DescriptorRanges;

    //

    EntityCollectPass::EntityCollectPass(
        Ecs::Universe&              universe,
        Cache::CommonPipelineState& commonPipelines) :
        m_Universe(universe),
        m_CommonPipelines(commonPipelines),
        m_MaxEntitiesCount(c_MinEntities)
    {
        m_CommonPipelines.get().Load(Cache::CommonPipelineState::Type::EntityCollectPass);

        Name("EntityCollectPass")
            .SetFlags(PassFlags::Compute)
            .Build(
                [this](Resolver& resolver)
                {
                    auto& world = *m_Universe.get().GetActiveWorld();

                    auto curEntityCount =
                        static_cast<uint32_t>(world.CreateFilter<
                                                       const Ecs::Component::BaseRenderable>()
                                                  .build()
                                                  .count());
                    m_MaxEntitiesCount = Math::AlignUp(std::max(curEntityCount, m_MaxEntitiesCount), c_MinEntities);

                    resolver.CreateBuffer(
                        Names::c_EntityCommandBuffer,
                        Rhi::BufferDesc{ Math::AlignUp(sizeof(DispatchDesc) * m_MaxEntitiesCount, c_BufferAlignment) });
                    resolver.CreateBuffer(
                        Names::c_EntityCommandCounter,
                        Rhi::BufferDesc{ Math::AlignUp(sizeof(int) * m_MaxEntitiesCount, c_BufferAlignment) });

                    //

                    resolver.ReadBuffer(
                        Names::c_TransformsTable("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER);
                    resolver.ReadBuffer(
                        Names::c_RenderInstancesTable("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER);

                    resolver.WriteBuffer(
                        Names::c_EntityCommandCounter("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER,
                        Rhi::ResourceFormat::R32_UINT);
                    resolver.WriteBuffer(
                        Names::c_EntityCommandBuffer("CollectPass"),
                        Rhi::ShaderType::COMPUTE_SHADER,
                        Rhi::ResourceFormat::R32_UINT);
                })
            .Execute(
                [this](const ResourceStorage& storage, Rhi::CommandList* commandList)
                {
                    auto& transformsTable      = storage.GetResourceViewHandle(Names::c_TransformsTable("CollectPass"));
                    auto& renderInstancesTable = storage.GetResourceViewHandle(Names::c_RenderInstancesTable("CollectPass"));
                    auto& commandsView         = storage.GetResourceViewHandle(Names::c_EntityCommandBuffer("CollectPass"));
                    auto& counterView          = storage.GetResourceViewHandle(Names::c_EntityCommandCounter("CollectPass"));

                    auto pipelineState = m_CommonPipelines.get()
                                             .Load(Cache::CommonPipelineState::Type::EntityCollectPass)
                                             .get();

                    commandList->SetPipelineLayout(pipelineState->GetLayout());

                    //

                    auto frameDataSet   = commandList->AllocateSet(CD::c_FrameData_SetIndex);
                    auto entityDataSet  = commandList->AllocateSet(CD::c_EntityData_SetIndex);
                    auto commandInfoSet = commandList->AllocateSet(c_CommandInfo_SetIndex);

                    nri::Descriptor* frameDescriptors[]{
                        storage.GetFrameResourceHandle().Unwrap()
                    };

                    nri::Descriptor* entityDescriptors[]{
                        transformsTable.Unwrap(),
                        renderInstancesTable.Unwrap()
                    };

                    nri::Descriptor* commandInfoDescriptors[]{
                        commandsView.Unwrap(),
                        counterView.Unwrap()
                    };

                    frameDataSet.SetRange(0, { frameDescriptors, Rhi::Count32(frameDescriptors) });
                    entityDataSet.SetRange(0, { entityDescriptors, Rhi::Count32(entityDescriptors) });
                    commandInfoSet.SetRange(0, { commandInfoDescriptors, Rhi::Count32(commandInfoDescriptors) });

                    //

                    commandList->SetDescriptorSet(CD::c_FrameData_SetIndex, frameDataSet);
                    commandList->SetDescriptorSet(CD::c_EntityData_SetIndex, entityDataSet);
                    commandList->SetDescriptorSet(c_CommandInfo_SetIndex, commandInfoSet);

                    //

                    commandList->SetPipelineState(pipelineState);

                    auto entityStore = storage.GetEntityStore();
                    for (auto& row : entityStore.GetCountedRows())
                    {
                        DispatchDesc DispatchConstants{
                            .DrawOffset    = row.DrawOffset,
                            .DrawCount     = row->Count,
                            .CounterOffset = row.CounterOffset
                        };
                        commandList->SetConstants(0, DispatchConstants);

                        commandList->ClearBuffer(
                            { .storageBuffer            = counterView.Unwrap(),
                              .setIndexInPipelineLayout = 2,
                              .rangeIndex               = 1,
                              .offsetInRange            = row.CounterOffset });

                        commandList->Dispatch(1);
                    }
                });
    }
} // namespace Ame::Gfx::RG::Std