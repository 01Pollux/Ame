#include <Rhi/Device/CommandListManager.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    CommandListManager::CommandInfo::CommandInfo(
        nri::CoreInterface& nriCore,
        nri::Device&        nriDevice,
        nri::CommandQueue&  nriCommandQueue,
        CommandQueueType    queueType)
    {
        ThrowIfFailed(
            nriCore.CreateCommandAllocator(nriCommandQueue, NriCommandAllocator),
            "Failed to create command allocator");

        ThrowIfFailed(
            nriCore.CreateCommandBuffer(*NriCommandAllocator, NriCommandBuffer),
            "Failed to create command buffer");

        ThrowIfFailed(
            nriCore.CreateFence(nriDevice, FenceValue, NriFence),
            "Failed to create fence");

#ifndef AME_DIST
        const char* queueName = nullptr;
        switch (queueType)
        {
        case CommandQueueType::GRAPHICS:
            queueName = "Graphics";
            break;
        case CommandQueueType::COMPUTE:
            queueName = "Compute";
            break;
        case CommandQueueType::COPY:
            queueName = "Transfer";
            break;
        default:
            std::unreachable();
        }

        auto commandAllocatorName = std::format("FrameCommandAllocator_{}", queueName);
        auto commandListName      = std::format("FrameCommandList_{}", queueName);
        auto fenceName            = std::format("FrameFence_{}", queueName);

        nriCore.SetCommandAllocatorDebugName(*NriCommandAllocator, commandAllocatorName.c_str());
        nriCore.SetCommandBufferDebugName(*NriCommandBuffer, commandListName.c_str());
        nriCore.SetFenceDebugName(*NriFence, fenceName.c_str());
#endif
    }

    //

    void CommandListManager::Shutdown(
        nri::CoreInterface& nriCore)
    {
        AME_LOG_ASSERT(Log::Rhi(), m_CommandInfoPools[0].PendingCommands.empty(), "There are still pending graphics command buffers");
        AME_LOG_ASSERT(Log::Rhi(), m_CommandInfoPools[1].PendingCommands.empty(), "There are still pending compute command buffers");
        AME_LOG_ASSERT(Log::Rhi(), m_CommandInfoPools[2].PendingCommands.empty(), "There are still pending copy command buffers");

        for (auto& pools : m_CommandInfoPools)
        {
            for (auto& commandInfo : pools.FreeCommands)
            {
                nriCore.DestroyFence(*commandInfo.NriFence);
                nriCore.DestroyCommandBuffer(*commandInfo.NriCommandBuffer);
                nriCore.DestroyCommandAllocator(*commandInfo.NriCommandAllocator);
            }
        }
    }

    void CommandListManager::SubmitAllCommands(
        nri::CoreInterface&   nriCore,
        nri::CommandQueue&    queue,
        nri::CommandQueueType queueType)
    {
        UploadCommands(nriCore, queue, queueType);
        PollAllCommands(nriCore, queueType);
    }

    //

    Co::result<void> CommandListManager::Submit(
        const SubmissionContext& submissionContext,
        std::vector<Fence>       waitFences)
    {
        auto& commandBuffer = submissionContext.CommandListRef.Unwrap();
        auto& fence         = submissionContext.FenceRef;

        auto& packets = m_PendingPackets[static_cast<uint32_t>(submissionContext.QueueType)];
        auto& packet  = packets.emplace_back(commandBuffer, fence, std::move(waitFences), Co::result_promise<void>{});
        return packet.Promise.get_result();
    }

    //

    void CommandListManager::UploadCommands(
        nri::CoreInterface& nriCore,
        nri::CommandQueue&  queue,
        CommandQueueType    queueType)
    {
        if (!CollectCommandBatches(queueType))
        {
            return;
        }
        SubmitCommandBatches(nriCore, queue);
        MoveCommandPackets(queueType);
    }

    bool CommandListManager::CollectCommandBatches(
        CommandQueueType queueType)
    {
        auto& packets = m_PendingPackets[static_cast<uint32_t>(queueType)];
        if (packets.empty())
        {
            return false;
        }

        m_CommandBuffersCache.reserve(packets.size());
        m_SignalFencesCache.reserve(packets.size());

        for (auto& packet : packets)
        {
            m_WaitFencesCache.reserve(m_WaitFencesCache.size() + packet.WaitFences.size());

            m_CommandBuffersCache.emplace_back(packet.CommandBuffer);

            for (auto& fence : packet.WaitFences)
            {
                m_WaitFencesCache.emplace_back(fence.Unwrap(), fence.GetValue(), fence.GetStages());
            }

            auto& fence = packet.SignalFence;
            m_SignalFencesCache.emplace_back(fence.Unwrap(), fence.GetValue(), fence.GetStages());
        }

        return true;
    }

    void CommandListManager::SubmitCommandBatches(
        nri::CoreInterface& nriCore,
        nri::CommandQueue&  queue)
    {
        nri::QueueSubmitDesc desc{
            .waitFences       = m_WaitFencesCache.data(),
            .waitFenceNum     = static_cast<uint32_t>(m_WaitFencesCache.size()),
            .commandBuffers   = m_CommandBuffersCache.data(),
            .commandBufferNum = static_cast<uint32_t>(m_CommandBuffersCache.size()),
            .signalFences     = m_SignalFencesCache.data(),
            .signalFenceNum   = static_cast<uint32_t>(m_SignalFencesCache.size())
        };

        nriCore.QueueSubmit(queue, desc);

        m_WaitFencesCache.clear();
        m_CommandBuffersCache.clear();
        m_SignalFencesCache.clear();
    }

    void CommandListManager::MoveCommandPackets(
        CommandQueueType queueType)
    {
        auto& pendingPackets   = m_PendingPackets[static_cast<uint32_t>(queueType)];
        auto& submittedPackets = m_SubmittedPackets[static_cast<uint32_t>(queueType)];

        std::ranges::move(pendingPackets, std::back_inserter(submittedPackets));
        pendingPackets.clear();
    }

    //

    void CommandListManager::PollAllCommands(
        nri::CoreInterface& nriCore,
        CommandQueueType    queueType)
    {
        auto& packets = m_SubmittedPackets[static_cast<uint32_t>(queueType)];
        std::erase_if(
            packets,
            [&](SumbissionPacket& packet)
            {
                bool allFinished = std::ranges::all_of(
                    packet.WaitFences,
                    [&](const Fence& fence)
                    {
                        return nriCore.GetFenceValue(*fence.Unwrap()) >= fence.GetValue();
                    });
                if (allFinished)
                {
                    packet.Promise.set_result();
                }
                return allFinished;
            });
    }
} // namespace Ame::Rhi