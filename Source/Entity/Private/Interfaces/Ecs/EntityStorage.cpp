#include <Interfaces/Ecs/Config.hpp>
#include <Interfaces/Ecs/EntityStorage.hpp>

#include <EcsModule/RenderableModule.hpp>
#include <EcsModule/Renderable3DModule.hpp>

#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Interfaces
{
    EntityStorage::EntityStorage(IReferenceCounters* counters, Ptr<Rhi::IRhiDevice> rhiDevice,
                                 const EntityStorageCreateDesc& storageDesc) :
        Base(counters),
        m_World(AmeCreate(Ecs::World, std::move(rhiDevice)))
    {
        auto& world = *GetWorld();
#ifndef AME_DIST
        if (storageDesc.EnableMonitors)
        {
            world->emplace<flecs::Rest>();
        }
#endif

        bool anyRenderable = storageDesc.Enable3DModule;
        if (anyRenderable)
        {
            world.ImportModule<Ecs::RenderableEcsModule>();
            if (storageDesc.Enable3DModule)
            {
                world.ImportModule<Ecs::Renderable3DEcsModule>();
            }
        }
    }

    //

    bool EntityStorage::Tick(double deltaTime)
    {
        auto& world = *GetWorld();
        world.Progress(deltaTime);
        return !world->should_quit();
    }
} // namespace Ame::Interfaces
