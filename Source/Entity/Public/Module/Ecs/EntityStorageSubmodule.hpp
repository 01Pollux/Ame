#pragma once

#include <Module/Submodule.hpp>

#include <Module/Ecs/Config.hpp>

namespace Ame
{
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
            const EcsModuleConfig& config);

    private:
        Ptr<Ecs::WorldObject> m_World;
    };
} // namespace Ame