#include <Module/Ecs/EntityStorageSubmodule.hpp>

#include <EcsModule/RenderableModule.hpp>
#include <EcsModule/Renderable3DModule.hpp>

#include <Module/Rhi/RhiModule.hpp>

namespace Ame
{
    [[nodiscard]] static Ptr<Rhi::IRhiDevice> GetRhiDevice(RhiModule* rhiModule)
    {
        Ptr<Rhi::IRhiDevice> rhiDevice;
        rhiModule->QueryInterface(Rhi::IID_RhiDevice, rhiDevice.DblPtr<IObject>());
        return rhiDevice;
    }

    EntityStorageSubmodule::EntityStorageSubmodule(
        IReferenceCounters*    counters,
        RhiModule*             rhiModule,
        const EcsModuleConfig& config) :
        Base(counters, IID_EntityModule),
        m_World(ObjectAllocator<Ecs::World>()(GetRhiDevice(rhiModule)))
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