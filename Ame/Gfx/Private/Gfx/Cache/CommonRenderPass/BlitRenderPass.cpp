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
        Rhi::CommandList commandList(m_Device);
        switch (QueryOptimalOperation(parameters))
        {
        case OptimalBlitOperation::Copy:
        {
            BlitCopy(commandList, parameters);
            break;
        }
        case OptimalBlitOperation::Render:
        {
            BlitRender(commandList, parameters);
            break;
        }
        default:
            std::unreachable();
        }
    }

    void BlitRenderPass::Blit(
        const SingleBlitParameters& parameters)
    {
        Blit(BlitParameters{
            .SrcTexture      = parameters.SrcTexture,
            .DstTexture      = parameters.DstTexture,
            .SrcRect         = parameters.SrcRect,
            .DstRect         = parameters.DstRect,
            .SrcSubresources = { &parameters.SrcSubresources, 1 },
            .DstSubresources = { &parameters.DstSubresources, 1 },
            .EnableAlpha     = parameters.EnableAlpha,
            .SwapRBChannels  = parameters.SwapRBChannels });
    }

    //

    auto BlitRenderPass::QueryOptimalOperation(
        const BlitParameters& parameters) -> OptimalBlitOperation
    {
        // If we don't need to enable alpha and we don't need to swap the red and blue channels, we can use a copy operation.
        if (!parameters.EnableAlpha && !parameters.SwapRBChannels)
        {
            // If any of the subresources are different, we can't use a copy operation.
            if (parameters.SrcSubresources.size() == parameters.DstSubresources.size())
            {
                for (size_t i = 0; i < parameters.SrcSubresources.size(); ++i)
                {
                    if (parameters.SrcSubresources[i] != parameters.DstSubresources[i])
                    {
                        return OptimalBlitOperation::Render;
                    }
                }

                return OptimalBlitOperation::Copy;
            }
        }
        return OptimalBlitOperation::Render;
    }

    //

    void BlitRenderPass::BlitCopy(
        Rhi::CommandList&     commandList,
        const BlitParameters& parameters)
    {
        auto resolvedSrcRect = parameters.SrcRect.Transform(parameters.SrcTexture);
        auto resolvedDstRect = parameters.DstRect.Transform(parameters.DstTexture);

        Rhi::TextureRegionDesc srcRegion{
            .x      = resolvedSrcRect.Position[0],
            .y      = resolvedSrcRect.Position[1],
            .z      = resolvedSrcRect.Position[2],
            .width  = resolvedSrcRect.Size[0],
            .height = resolvedSrcRect.Size[1],
            .depth  = resolvedSrcRect.Size[2]
        };
        Rhi::TextureRegionDesc dstRegion{
            .x      = resolvedDstRect.Position[0],
            .y      = resolvedDstRect.Position[1],
            .z      = resolvedDstRect.Position[2],
            .width  = resolvedDstRect.Size[0],
            .height = resolvedDstRect.Size[1],
            .depth  = resolvedDstRect.Size[2]
        };

        for (auto [src, dst] : std::views::zip(parameters.SrcSubresources, parameters.DstSubresources))
        {
            auto resolvedSrc = src.Transform(parameters.SrcTexture);
            auto resolvedDst = dst.Transform(parameters.DstTexture);

            for (uint32_t i = 0; i < resolvedSrc.Mips.Count; ++i)
            {
                srcRegion.mipOffset = resolvedSrc.Mips.Offset + i;
                dstRegion.mipOffset = resolvedDst.Mips.Offset + i;

                for (uint32_t j = 0; j < resolvedSrc.Array.Count; ++j)
                {
                    srcRegion.arrayOffset = resolvedSrc.Array.Offset + j;
                    dstRegion.arrayOffset = resolvedDst.Array.Offset + j;

                    commandList.CopyTexture(
                        {
                            .RhiTexture = parameters.SrcTexture,
                            .Region     = &srcRegion,
                        },
                        {
                            .RhiTexture = parameters.DstTexture,
                            .Region     = &dstRegion,
                        });
                }
            }
        }
    }

    //

    void BlitRenderPass::BlitRender(
        Rhi::CommandList&     commandList,
        const BlitParameters& parameters)
    {
    }
} // namespace Ame::Gfx::Cache