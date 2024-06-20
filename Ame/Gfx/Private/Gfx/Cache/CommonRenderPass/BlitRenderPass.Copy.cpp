#include <ranges>

#include <Rhi/CommandList/CommandList.hpp>
#include <Gfx/Cache/CommonRenderPass/BlitRenderPass.hpp>

namespace Ame::Gfx::Cache
{
    void BlitRenderPass::BlitPushCopyBarrier(
        BlitOperation& operation)
    {
        if (!operation.Parameters.SrcTransition &&
            !operation.Parameters.DstTransition)
        {
            return;
        }

        std::vector<Rhi::TextureBarrierDesc> textureBarriers;
        textureBarriers.reserve(operation.SrcSubresources.size() + operation.DstSubresources.size());

        for (auto [srcSubresource, dstSubresource] : std::views::zip(operation.SrcSubresources, operation.DstSubresources))
        {
            if (operation.Parameters.SrcTransition)
            {
                textureBarriers.emplace_back(Rhi::TextureBarrierDesc{
                    .texture = operation.Parameters.SrcTexture.get().Unwrap(),
                    .before  = operation.Parameters.OldSrcState,
                    .after{ .access = Rhi::AccessBits::COPY_SOURCE, .layout = Rhi::LayoutType::COPY_SOURCE, .stages = Rhi::StageBits::COPY },
                });
            }

            if (operation.Parameters.DstTransition)
            {
                textureBarriers.emplace_back(Rhi::TextureBarrierDesc{
                    .texture = operation.Parameters.DstTexture.get().Unwrap(),
                    .before  = operation.Parameters.OldDstState,
                    .after{ .access = Rhi::AccessBits::COPY_DESTINATION, .layout = Rhi::LayoutType::COPY_DESTINATION, .stages = Rhi::StageBits::COPY },
                });
            }
        }

        Rhi::BarrierGroupDesc barrier{
            .textures   = textureBarriers.data(),
            .textureNum = static_cast<uint16_t>(textureBarriers.size()),
        };
        operation.CommandList.get().ResourceBarrier(barrier);
    }

    void BlitRenderPass::BlitPopCopyBarrier(
        BlitOperation& operation)
    {
        if (!operation.Parameters.SrcTransition &&
            !operation.Parameters.DstTransition)
        {
            return;
        }

        std::vector<Rhi::TextureBarrierDesc> textureBarriers;
        textureBarriers.reserve(operation.SrcSubresources.size() + operation.DstSubresources.size());

        for (auto [srcSubresource, dstSubresource] : std::views::zip(operation.SrcSubresources, operation.DstSubresources))
        {
            if (operation.Parameters.SrcTransition)
            {
                textureBarriers.emplace_back(Rhi::TextureBarrierDesc{
                    .texture = operation.Parameters.SrcTexture.get().Unwrap(),
                    .before{ .access = Rhi::AccessBits::COPY_SOURCE, .layout = Rhi::LayoutType::COPY_SOURCE, .stages = Rhi::StageBits::COPY },
                    .after = operation.Parameters.OldSrcState });
            }

            if (operation.Parameters.DstTransition)
            {
                textureBarriers.emplace_back(Rhi::TextureBarrierDesc{
                    .texture = operation.Parameters.DstTexture.get().Unwrap(),
                    .before{ .access = Rhi::AccessBits::COPY_DESTINATION, .layout = Rhi::LayoutType::COPY_DESTINATION, .stages = Rhi::StageBits::COPY },
                    .after = operation.Parameters.OldDstState });
            }
        }

        Rhi::BarrierGroupDesc barrier{
            .textures   = textureBarriers.data(),
            .textureNum = static_cast<uint16_t>(textureBarriers.size()),
        };
        operation.CommandList.get().ResourceBarrier(barrier);
    }

    //

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

                    operation.CommandList.get().CopyTexture(
                        { .NriTexture = operation.Parameters.SrcTexture.get().Unwrap(),
                          .Region     = srcRegion },
                        { .NriTexture = operation.Parameters.DstTexture.get().Unwrap(),
                          .Region     = dstRegion });
                }
            }
        }
    }
} // namespace Ame::Gfx::Cache