#pragma once

#include <RG/Pass.hpp>

namespace Ame::Gfx
{
    class RecordIndirectCommandsPass : public RG::Pass
    {
    public:
        RecordIndirectCommandsPass();

    private:
        void Build(
            RG::Resolver& resolver);

    private:
    };
} // namespace Ame::Gfx