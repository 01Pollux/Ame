#pragma once

#include <RG/Pass.hpp>

namespace Ame::Gfx
{
    class InitializeForwardPlusPass : public RG::Pass
    {
    public:
        InitializeForwardPlusPass();

    private:
        void Build(
            RG::Resolver& resolver);
    };
} // namespace Ame::Gfx