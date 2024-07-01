#include <ranges>

#include <Gfx/RenderGraph/Passes/ClearBackbufferPass.hpp>

namespace Ame::Gfx
{
    ClearBackbufferPass::ClearBackbufferPass()
    {
        Name("ClearTexturePass")
            .SetFlags(RG::PassFlags::Graphics)
            .Build(
                [this](RG::Resolver& resolver)
                {
                    resolver.WriteRenderTarget(RG::Names::c_BackbufferImage("Clear"), Rhi::StageBits::GRAPHICS_SHADERS);
                });
    }
} // namespace Ame::Gfx