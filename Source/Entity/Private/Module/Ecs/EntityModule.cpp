#include <Module/Ecs/EntityModule.hpp>

namespace Ame
{
    EntityModule::EntityModule(
        IReferenceCounters*    counters,
        Dependencies           dependencies,
        const EcsModuleConfig& config) :
        Base(counters, IID_EntityModule),
        m_EntityStorageSubmodule(ObjectAllocator<EntityStorageSubmodule>()(dependencies.RhiMod, config))
    {
    }
} // namespace Ame