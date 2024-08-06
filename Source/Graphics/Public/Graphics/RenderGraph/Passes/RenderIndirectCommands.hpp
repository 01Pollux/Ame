#pragma once

#include <RG/Pass.hpp>

namespace Ame::Gfx
{
    class RenderIndirectCommandsPass : public RG::Pass
    {
    public:
        RenderIndirectCommandsPass();

    private:
        void Build(
            RG::Resolver& resolver);

    private:
    };
} // namespace Ame::Gfx