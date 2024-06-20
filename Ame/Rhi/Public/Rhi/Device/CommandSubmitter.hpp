#pragma once

#include <Core/Coroutine.hpp>

#include <Rhi/CommandList/SubmissionContext.hpp>

namespace Ame::Rhi
{
    class DeviceCommandSubmitter
    {
    public:
        DeviceCommandSubmitter(
            class DeviceImpl& rhiDeviceImpl);

    public:
        /// <summary>
        /// Begins a new command list for the specified queue type.
        /// </summary>
        [[nodiscard]] Co::result<SubmissionContext> BeginCommandList(
            CommandQueueType queueType,
            StageBits        stages = StageBits::ALL);

        /// <summary>
        /// Submits the command list to the device.
        /// </summary>
        [[nodiscard]] Co::result<void> SubmitCommandList(
            const SubmissionContext& submissionContext);

        /// <summary>
        /// Submits the command list to the device.
        /// </summary>
        [[nodiscard]] Co::result<void> SubmitCommandList(
            const SubmissionContext&                  submissionContext,
            const std::span<CRef<SubmissionContext>>& dependencies);

    private:
        Ref<DeviceImpl> m_RhiDeviceImpl;
    };
} // namespace Ame::Rhi