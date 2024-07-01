#include <ranges>

#include <Gfx/RenderGraph/Passes/PresentBackbufferPass.hpp>

namespace Ame::Gfx
{
    PresentBackbufferPass::PresentBackbufferPass()
    {
        Name("PresentBackbufferPass")
            .SetFlags(RG::PassFlags::Graphics)
            .Build(
                [this](RG::Resolver& resolver)
                {
                    resolver.WritePresentResource(RG::Names::c_BackbufferImage("Final"));
                });
    }
} // namespace Ame::Gfx