#include <Rhi/Device/CommandSubmitter.hpp>
#include <Rhi/Device/Device/Device.Impl.hpp>

namespace Ame::Rhi
{
    DeviceCommandSubmitter::DeviceCommandSubmitter(
        DeviceImpl& rhiDeviceImpl) :
        m_RhiDeviceImpl(rhiDeviceImpl)
    {
    }

    SubmissionContext DeviceCommandSubmitter::BeginCommandList(
        CommandQueueType queueType,
        StageBits        stages)
    {
        m_RhiDeviceImpl.get().AssertRenderingThread();

        auto& deviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& frameManager  = m_RhiDeviceImpl.get().GetFrameManager();
        auto& frame         = frameManager.GetCurrentFrame();
        auto& nri           = deviceWrapper.GetNri();

        auto& nriCore      = *nri.GetCoreInterface();
        auto& device       = deviceWrapper.GetNriDevice();
        auto& commandQueue = deviceWrapper.GetQueue(queueType);

        auto submission = frame.AllocateSubmission(nriCore, device, commandQueue, queueType, stages);
        nriCore.BeginCommandBuffer(*submission.CommandListRef.Unwrap(), nullptr);

        return submission;
    }

    Co::result<void> DeviceCommandSubmitter::SubmitCommandList(
        const SubmissionContext& submissionContext)
    {
        m_RhiDeviceImpl.get().AssertRenderingThread();

        auto& deviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri           = deviceWrapper.GetNri();
        auto& nriCore       = *nri.GetCoreInterface();

        nriCore.EndCommandBuffer(*submissionContext.CommandListRef.Unwrap());

        auto& commandListManager = m_RhiDeviceImpl.get().GetCommandListManager();
        return commandListManager.Submit(submissionContext);
    }

    Co::result<void> DeviceCommandSubmitter::SubmitCommandList(
        const SubmissionContext&                  submissionContext,
        const std::span<CRef<SubmissionContext>>& dependencies)
    {
        auto waitFences = dependencies |
                          std::views::transform([](const SubmissionContext& context)
                                                { return context.FenceRef; }) |
                          std::ranges::to<std::vector>();
        return SubmitCommandList(submissionContext, std::move(waitFences));
    }

    Co::result<void> DeviceCommandSubmitter::SubmitCommandList(
        const SubmissionContext& submissionContext,
        std::vector<Rhi::Fence>  dependencies)
    {
        m_RhiDeviceImpl.get().AssertRenderingThread();

        auto& deviceWrapper = m_RhiDeviceImpl.get().GetDeviceWrapper();
        auto& nri           = deviceWrapper.GetNri();
        auto& nriCore       = *nri.GetCoreInterface();

        nriCore.EndCommandBuffer(*submissionContext.CommandListRef.Unwrap());
        auto& commandListManager = m_RhiDeviceImpl.get().GetCommandListManager();

        return commandListManager.Submit(submissionContext, std::move(dependencies));
    }
} // namespace Ame::Rhi