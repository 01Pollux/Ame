#include <Graphics/RenderGraph/Passes/RecordIndirectCommands.hpp>

#include <Shaders/RecordIndirectCommandsShader.hpp>
#include <Rhi/Utils/DeviceWithCache.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>

#include <Ecs/World.hpp>
#include <Graphics/RenderGraph/Common/EntityIterator.hpp>

namespace Ame::Gfx
{
    RecordIndirectCommandsPass::RecordIndirectCommandsPass(
        Dg::IRenderDevice* renderDevice,
        Ecs::World*        world) :
        m_World(world)
    {
        Name("Record Indirect Commands Pass")
            .Flags(RG::PassFlags::Graphics)
            .Build(std::bind_front(&RecordIndirectCommandsPass::Build, this))
            .Execute(std::bind_front(&RecordIndirectCommandsPass::Execute, this));

        //

        Rhi::RenderDeviceWithCache<> renderDeviceWithCache(renderDevice);

        Ptr<Dg::IShader> computeShader;
        {
            Rhi::RecordIndirectCommandsShader shader;
            computeShader = renderDeviceWithCache.CreateShader(shader);
        }

        Dg::ComputePipelineStateCreateInfo pipelineStateInfo{ "RecordIndirectCommandsPSO" };
        pipelineStateInfo.pCS = computeShader;

        pipelineStateInfo.PSODesc.ResourceLayout.DefaultVariableType = Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC;

        Dg::ShaderResourceVariableDesc variables[]{
            { Dg::SHADER_TYPE_COMPUTE, "DispatchConstants", Dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC }
        };
        pipelineStateInfo.PSODesc.ResourceLayout.Variables    = variables;
        pipelineStateInfo.PSODesc.ResourceLayout.NumVariables = Rhi::Count32(variables);

        m_PipelineState = renderDeviceWithCache.CreateComputePipelineState(pipelineStateInfo);

        //

        Dg::BufferDesc bufferDesc{
            "Indirect Commands Dispatch Constants",
            sizeof(DispatchConstants),
            Dg::BIND_UNIFORM_BUFFER,
            Dg::USAGE_DYNAMIC,
            Dg::CPU_ACCESS_WRITE
        };
        renderDevice->CreateBuffer(bufferDesc, nullptr, &m_DispatchConstants);

        //

        m_PipelineState->GetStaticVariableByName(Dg::SHADER_TYPE_COMPUTE, "DispatchConstants")->Set(m_DispatchConstants);
        m_PipelineState->CreateShaderResourceBinding(&m_Srb, true);
    }

    void RecordIndirectCommandsPass::Build(
        RG::Resolver& resolver)
    {
        resolver.ReadBuffer(RGRenderInstances, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);
        resolver.ReadBuffer(RGSortedRenderInstances, Dg::BIND_SHADER_RESOURCE, Dg::BUFFER_VIEW_SHADER_RESOURCE);

        resolver.WriteBuffer(RGDrawCommands, Dg::BIND_UNORDERED_ACCESS, Dg::BUFFER_VIEW_UNORDERED_ACCESS);
        resolver.WriteBuffer(RGDrawCommandCounts, Dg::BIND_UNORDERED_ACCESS, Dg::BUFFER_VIEW_UNORDERED_ACCESS);
    }

    void RecordIndirectCommandsPass::Execute(
        const RG::ResourceStorage& storage,
        Dg::IDeviceContext*        deviceContext)
    {
        CRef renderInstances       = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGRenderInstances));
        CRef sortedRenderInstances = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGSortedRenderInstances));

        CRef drawCommands      = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGDrawCommands));
        CRef drawCommandCounts = std::get<RG::RhiBufferViewRef>(storage.GetResourceView(RGDrawCommandCounts));

        //

        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "RenderInstances")->Set(renderInstances.get().View);
        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "SortedRenderInstances")->Set(sortedRenderInstances.get().View);

        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "DrawCommands")->Set(drawCommands.get().View);
        m_Srb->GetVariableByName(Dg::SHADER_TYPE_COMPUTE, "DrawCommandCounts")->Set(drawCommandCounts.get().View);

        //

        auto& world            = *m_World;
        auto  commandsIterator = world->get<EntityDrawCommandsCategoryIterator>();

        Dg::MapHelper<DispatchConstants> dispatchConstants;
        for (auto& group : commandsIterator->GetGroups())
        {
            for (auto& command : group.GetCommands())
            {
                if (!dispatchConstants) [[unlikely]]
                {
                    dispatchConstants.Map(deviceContext, m_DispatchConstants, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);
                }

                deviceContext->SetPipelineState(m_PipelineState);

                dispatchConstants->FirstInstance = command.CounterOffset; // TODO: grouped instance in command
                dispatchConstants->InstanceCount = 1;
                dispatchConstants->CounterOffset = command.CounterOffset;

                deviceContext->CommitShaderResources(m_Srb, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                deviceContext->DispatchCompute({ 1, 1, 1 }); // TODO: [command.size() / BLOCK, 1, 1]
            }
        }
    }
} // namespace Ame::Gfx