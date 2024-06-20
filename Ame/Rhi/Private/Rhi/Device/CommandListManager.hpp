#pragma once

#include <Core/Ame.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/Frame.hpp>

namespace Ame::Rhi
{
    class CommandListManager
    {
        struct CommandInfo
        {
            nri::CommandAllocator* NriCommandAllocator = nullptr;
            nri::CommandBuffer*    NriCommandBuffer    = nullptr;
            nri::Fence*            NriFence            = nullptr;
            uint64_t               FenceValue          = 0;

            CommandInfo(
                nri::CoreInterface& nriCore,
                nri::Device&        nriDevice,
                nri::CommandQueue&  nriCommandQueue,
                CommandQueueType    queueType);
        };

        struct CommandInfoPool
        {
            std::vector<CommandInfo> FreeCommands;
            std::vector<CommandInfo> PendingCommands;
        };

        struct SumbissionPacket
        {
            nri::CommandBuffer*      CommandBuffer;
            Fence                    SignalFence;
            std::vector<Fence>       WaitFences;
            Co::result_promise<void> Promise;
        };

        struct SubmissionPacketBatch
        {
            std::vector<nri::Fence*>         WaitFences;
            std::vector<nri::Fence*>         SignalFences;
            std::vector<nri::CommandBuffer*> CommandBuffers;
        };

        static constexpr uint32_t c_MaxCommandQueues = static_cast<uint32_t>(CommandQueueType::MAX_NUM);
        using CommandInfoPools                       = std::array<CommandInfoPool, c_MaxCommandQueues>;
        using SubmissionPackets                      = std::array<std::vector<SumbissionPacket>, c_MaxCommandQueues>;

    public:
        /// <summary>
        /// Shuts down the command list manager.
        /// </summary>
        /// <param name="nriCore"></param>
        void Shutdown(
            nri::CoreInterface& nriCore);

        /// <summary>
        /// Polls and upload the submission packets.
        /// </summary>
        void SubmitAllCommands(
            nri::CoreInterface&   nriCore,
            nri::CommandQueue&    queue,
            nri::CommandQueueType queueType);

    public:
        /// <summary>
        /// Submits the given submission context.
        /// </summary>
        [[nodiscard]] Co::result<void> Submit(
            const SubmissionContext& submissionContext,
            std::vector<Fence>       waitFences = {});

    private:
        /// <summary>
        /// Polls and upload the submission packets.
        /// </summary>
        void UploadCommands(
            nri::CoreInterface& nriCore,
            nri::CommandQueue&  queue,
            CommandQueueType    queueType);

        /// <summary>
        /// Collects all the command batches.
        /// returns true if there are any pending packets, false otherwise.
        /// </summary>
        [[nodiscard]] bool CollectCommandBatches(
            CommandQueueType queueType);

        /// <summary>
        /// Submits all the command batches.
        /// </summary>
        void SubmitCommandBatches(
            nri::CoreInterface& nriCore,
            nri::CommandQueue&  queue);

        /// <summary>
        /// Moves the command packets from the pending to the submitted packets.
        /// </summary>
        void MoveCommandPackets(
            CommandQueueType queueType);

        /// <summary>
        /// Polls all the commands and resume their execution.
        /// </summary>
        void PollAllCommands(
            nri::CoreInterface& nriCore,
            CommandQueueType    queueType);

    private:
        CommandInfoPools  m_CommandInfoPools;
        SubmissionPackets m_PendingPackets;
        SubmissionPackets m_SubmittedPackets;

        std::vector<nri::CommandBuffer*>  m_CommandBuffersCache;
        std::vector<nri::FenceSubmitDesc> m_WaitFencesCache;
        std::vector<nri::FenceSubmitDesc> m_SignalFencesCache;
    };
} // namespace Ame::Rhi