#include <ranges>

#include <Rhi/CommandList/CommandList.hpp>
#include <Gfx/Cache/CommonRenderPass/BlitRenderPass.hpp>

namespace Ame::Gfx::Cache
{
    void BlitRenderPass::BlitCopyBarrier(
        BlitOperation& operation)
    {
        if (!operation.Parameters.PlacePreBarrier)
        {
            return;
        }

        for (auto [srcSubresource, dstSubresource] : std::views::zip(operation.SrcSubresources, operation.DstSubresources))
        {
            operation.CommandList.RequireState(
                operation.Parameters.SrcTexture.get().Unwrap(),
                Rhi::AccessLayoutStage{
                    .access = Rhi::AccessBits::COPY_SOURCE,
                    .layout = Rhi::LayoutType::COPY_SOURCE,
                    .stages = Rhi::StageBits::COPY },
                srcSubresource);

            operation.CommandList.RequireState(
                operation.Parameters.DstTexture.get().Unwrap(),
                Rhi::AccessLayoutStage{
                    .access = Rhi::AccessBits::COPY_DESTINATION,
                    .layout = Rhi::LayoutType::COPY_DESTINATION,
                    .stages = Rhi::StageBits::COPY },
                dstSubresource);
        }

        operation.CommandList.CommitBarriers();
    }

    void BlitRenderPass::BlitCopy(
        BlitOperation& operation)
    {
        Rhi::TextureRegionDesc srcRegion{
            .x      = operation.SrcRect.Position[0],
            .y      = operation.SrcRect.Position[1],
            .z      = operation.SrcRect.Position[2],
            .width  = operation.SrcRect.Size[0],
            .height = operation.SrcRect.Size[1],
            .depth  = operation.SrcRect.Size[2]
        };
        Rhi::TextureRegionDesc dstRegion{
            .x      = operation.DstRect.Position[0],
            .y      = operation.DstRect.Position[1],
            .z      = operation.DstRect.Position[2],
            .width  = operation.DstRect.Size[0],
            .height = operation.DstRect.Size[1],
            .depth  = operation.DstRect.Size[2]
        };

        for (auto [srcSubresource, dstSubresource] : std::views::zip(operation.SrcSubresources, operation.DstSubresources))
        {
            for (uint32_t i = 0; i < srcSubresource.Mips.Count; ++i)
            {
                srcRegion.mipOffset = srcSubresource.Mips.Offset + i;
                dstRegion.mipOffset = dstSubresource.Mips.Offset + i;

                for (uint32_t j = 0; j < srcSubresource.Array.Count; ++j)
                {
                    srcRegion.arrayOffset = srcSubresource.Array.Offset + j;
                    dstRegion.arrayOffset = dstSubresource.Array.Offset + j;

                    operation.CommandList.CopyTexture(
                        { .NriTexture = operation.Parameters.SrcTexture.get().Unwrap(),
                          .Region     = srcRegion },
                        { .NriTexture = operation.Parameters.DstTexture.get().Unwrap(),
                          .Region     = dstRegion });
                }
            }
        }
    }
} // namespace Ame::Gfx::Cache