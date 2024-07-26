#include <Module/Ecs/EntityModule.hpp>

namespace Ame
{
    EntityModule::EntityModule(
        IReferenceCounters*    counters,
        const EcsModuleConfig& config) :
        Base(counters, IID_EntityModule),
        m_EntityStorageSubmodule(ObjectAllocator<EntityStorageSubmodule>()(config))
    {
    }
} // namespace Ame