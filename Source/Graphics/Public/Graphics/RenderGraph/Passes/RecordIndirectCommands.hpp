#pragma once

#include <RG/Pass.hpp>

namespace Ame::Ecs
{
    class World;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    class RecordIndirectCommandsPass : public RG::Pass
    {
    public:
        RecordIndirectCommandsPass(
            Ecs::World* world);

    private:
        void Build(
            RG::Resolver& resolver);

        void Execute(
            const RG::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        Ptr<Ecs::World> m_World;
    };
} // namespace Ame::Gfx