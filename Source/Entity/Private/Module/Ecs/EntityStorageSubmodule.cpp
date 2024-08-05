#include <Module/Ecs/EntityStorageSubmodule.hpp>

#include <EcsModule/RenderableModule.hpp>
#include <EcsModule/Renderable3DModule.hpp>

namespace Ame
{
    EntityStorageSubmodule::EntityStorageSubmodule(
        IReferenceCounters*    counters,
        const EcsModuleConfig& config) :
        Base(counters, IID_EntityModule),
        m_World(ObjectAllocator<Ecs::World>()())
    {
        auto& world = *m_World;

#ifndef AME_DIST
        if (config.EnableMonitors)
        {
            world->emplace<flecs::Rest>();
        }
#endif

        bool anyRenderable = config.Enable3DModule;
        if (anyRenderable)
        {
            world.ImportModule<Ecs::RenderableEcsModule>();
            if (config.Enable3DModule)
            {
                world.ImportModule<Ecs::Renderable3DEcsModule>();
            }
        }
    }
} // namespace Ame