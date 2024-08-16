#pragma once

#include <RG/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>

namespace Ame::Gfx
{
    class ForwardPlus_InitializePass : public RG::Pass
    {
    public:
        ForwardPlus_InitializePass();

    private:
        void OnBuild(
            RG::Resolver& resolver);
    };
} // namespace Ame::Gfx