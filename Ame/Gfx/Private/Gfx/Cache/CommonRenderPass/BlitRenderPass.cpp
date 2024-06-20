#include <ranges>

#include <Rhi/CommandList/CommandList.hpp>
#include <Gfx/Cache/CommonRenderPass/BlitRenderPass.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/CommandSubmitter.hpp>

namespace Ame::Gfx::Cache
{
    BlitRenderPass::BlitRenderPass(
        Rhi::Device& rhiDevice) :
        m_RhiDevice(rhiDevice)
    {
    }

    Co::result<void> BlitRenderPass::Blit(
        const BlitParameters& parameters)
    {
        auto& commandSubmitter = m_RhiDevice.get().GetCommandSubmitter();

        auto submission = commandSubmitter.BeginCommandList(Rhi::CommandQueueType::GRAPHICS);
        Blit(submission.CommandListRef, parameters);
        return commandSubmitter.SubmitCommandList(submission);
    }

    void BlitRenderPass::Blit(
        Rhi::CommandList&     commandList,
        const BlitParameters& parameters)
    {
        BlitOperation operation(commandList, parameters);
        switch (operation.OptimalType)
        {
        case OptimalBlitOperation::Copy:
        {
            BlitPushCopyBarrier(operation);
            BlitCopy(operation);
            BlitPopCopyBarrier(operation);
            break;
        }
        case OptimalBlitOperation::Render:
        {
            BlitPushRenderBarrier(operation);
            BlitRender(operation);
            BlitPopRenderBarrier(operation);
            break;
        }
        default:
            std::unreachable();
        }
    }
} // namespace Ame::Gfx::Cache