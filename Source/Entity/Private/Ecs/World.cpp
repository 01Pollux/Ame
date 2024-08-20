#include <mutex>
#include <Ecs/World.hpp>

#include <EcsComponent/Core/EntityTagComponent.hpp>
#include <EcsModule/CoreModule.hpp>

#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// The mutex for initializing flecs world.
    /// It is used to prevent multiple threads from initializing the world at the same time.
    /// </summary>
    static std::mutex g_FlecsMutex;

    World::World(
        IReferenceCounters*         counter,
        const Ptr<Rhi::IRhiDevice>& rhiDevice) :
        Base(counter),
        m_RhiDevice(rhiDevice)
    {
        std::lock_guard initLock(g_FlecsMutex);
        m_World = std::make_unique<flecs::world>();
        m_World->component<This>();
        m_World->set(This{ this });
        ImportModule<CoreEcsModule>();
    }

    World::~World()
    {
        if (m_World)
        {
            std::lock_guard Lock(g_FlecsMutex);
            m_World.reset();
        }
    }

    //

    static Entity CreateEntityFromWorld(
        flecs::world& world,
        StringView    name,
        const Entity& parent)
    {
        Entity entity(
            world.entity()
                .emplace<EntityTagComponent>());
        if (parent)
        {
            entity.SetParent(parent);
        }
        entity.SetName(String{ name });
        return entity;
    }

    //

    Entity World::CreateEntity(
        StringView    name,
        const Entity& parent)
    {
        return CreateEntityFromWorld(*m_World, name, parent);
    }

    Entity World::GetEntityById(
        const EntityId id) const
    {
        return Entity(m_World->entity(id));
    }

    //

    Entity WorldRef::CreateEntity(
        StringView    name,
        const Entity& parent)
    {
        return CreateEntityFromWorld(m_World, name, parent);
    }

    Entity WorldRef::GetEntityById(
        const EntityId id) const
    {
        return Entity(m_World.entity(id));
    }

    //

    void World::Progress(
        [[maybe_unused]] double deltaTime)
    {
#ifdef AME_DIST
        m_World->progress(static_cast<float>(deltaTime));
#else
        m_World->progress();
#endif
    }
} // namespace Ame::Ecs