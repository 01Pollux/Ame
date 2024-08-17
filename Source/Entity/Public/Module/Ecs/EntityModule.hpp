#pragma once

#include <Module/Module.hpp>

#include <Module/Ecs/Config.hpp>
#include <Module/Ecs/EntityStorageSubmodule.hpp>

namespace Ame
{
    class EntityModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_EntityModule, IID_BaseModule, Base,
            m_EntityStorageSubmodule);

        struct Dependencies
        {
            RhiModule* RhiMod;
        };

    public:
        EntityModule(
            IReferenceCounters*    counters,
            Dependencies           dependencies,
            const EcsModuleConfig& config);

    private:
        Ptr<EntityStorageSubmodule> m_EntityStorageSubmodule;
    }; // namespace Ame
} // namespace Ame