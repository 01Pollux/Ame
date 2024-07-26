#pragma once

#include <Module/Module.hpp>

#include <Module/Ecs/Config.hpp>

namespace Ame
{
    class RhiModule;
    class GraphicsModule;

    class EntityModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_EntityModule, IID_BaseModule, Base,
            nullptr);

    public:
        EntityModule(
            IReferenceCounters*    counters,
            const EcsModuleConfig& config);

    private:
    }; // namespace Ame
} // namespace Ame