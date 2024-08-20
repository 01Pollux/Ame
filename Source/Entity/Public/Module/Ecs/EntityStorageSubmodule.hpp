#pragma once

#include <Module/Submodule.hpp>

#include <Module/Ecs/Config.hpp>

namespace Ame
{
    class RhiModule;

    class EntityStorageSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_EntityStorageSubmodule, IID_BaseSubmodule, Base,
            m_World);

    public:
        EntityStorageSubmodule(
            IReferenceCounters*    counters,
            RhiModule*             rhiModule,
            const EcsModuleConfig& config);

    public:
        [[nodiscard]] bool Tick(
            double deltaTime);

    private:
        Ptr<Ecs::World> m_World;
    };
} // namespace Ame