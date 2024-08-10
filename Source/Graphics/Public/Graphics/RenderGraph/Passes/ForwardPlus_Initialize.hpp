#pragma once

#include <RG/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>

namespace Ame::Gfx
{
    class ForwardPlus_InitializePass : public RG::Pass
    {
    public:
        static inline const auto RGFinalImage = c_RGFinalImage("Initialize");

    public:
        ForwardPlus_InitializePass();

    private:
        void Build(
            RG::Resolver& resolver);
    };
} // namespace Ame::Gfx