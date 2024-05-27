#include <ranges>

#include <Rhi/CommandList/CommandList.hpp>
#include <Gfx/Cache/CommonRenderPass/BlitRenderPass.hpp>

namespace Ame::Gfx::Cache
{
    BlitRenderPass::BlitOperation::BlitOperation(
        Rhi::Device&          rhiDevice,
        const BlitParameters& parameters) :
        CommandList(rhiDevice),
        Parameters(parameters),
        SrcRect(parameters.SrcRect.Transform(parameters.SrcTexture)),
        DstRect(parameters.DstRect.Transform(parameters.DstTexture)),
        SrcSubresources(ResolveSubresources(parameters, parameters.SrcSubresources)),
        DstSubresources(ResolveSubresources(parameters, parameters.DstSubresources)),
        OptimalType(QueryOptimalOperation(parameters))
    {
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

    std::vector<Rhi::TextureSubresource> BlitRenderPass::ResolveSubresources(
        const BlitParameters&                    parameters,
        std::span<const Rhi::TextureSubresource> subresources)
    {
        return subresources |
               std::views::transform(
                   [&parameters](auto& subresource)
                   { return subresource.Transform(parameters.SrcTexture); }) |
               std::ranges::to<std::vector>();
    }
} // namespace Ame::Gfx::Cache