#pragma once

#include <Rg/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>

namespace Ame::Gfx
{
    class ForwardPlus_InitializePass : public Rg::Pass
    {
    public:
        ForwardPlus_InitializePass();

    private:
        void OnBuild(
            Rg::Resolver& resolver);
    };
} // namespace Ame::Gfx