#include <ranges>

#include <Rhi/CommandList/CommandList.hpp>
#include <Gfx/Cache/CommonRenderPass/BlitRenderPass.hpp>

namespace Ame::Gfx::Cache
{
    BlitRenderPass::BlitRenderPass(
        Rhi::Device& rhiDevice) :
        m_Device(rhiDevice)
    {
    }

    //

    void BlitRenderPass::Blit(
        const BlitParameters& parameters)
    {
        BlitOperation operation(m_Device, parameters);
        switch (operation.OptimalType)
        {
        case OptimalBlitOperation::Copy:
        {
            BlitCopyBarrier(operation);
            BlitCopy(operation);
            break;
        }
        case OptimalBlitOperation::Render:
        {
            BlitRenderBarrier(operation);
            BlitRender(operation);
            break;
        }
        default:
            std::unreachable();
        }
        BlitRestoreState(operation);
    }

    //

    void BlitRenderPass::BlitRestoreState(
        BlitOperation& operation)
    {
        if (operation.Parameters.PlacePostBarrier)
        {
            operation.CommandList.RequireState(
                operation.Parameters.DstTexture,
                operation.Parameters.NewState);

            if (operation.Parameters.FlushPostBarrier)
            {
                operation.CommandList.CommitBarriers();
            }
        }
    }
} // namespace Ame::Gfx::Cache