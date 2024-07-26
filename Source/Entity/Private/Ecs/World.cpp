#include <mutex>
#include <Ecs/World.hpp>

#include <EcsModule/CoreModule.hpp>
#include <EcsComponent/Core/EntityTagComponent.hpp>

namespace Ame::Ecs
{
    /// <summary>
    /// The mutex for initializing flecs world.
    /// It is used to prevent multiple threads from initializing the world at the same time.
    /// </summary>
    static std::mutex g_FlecsMutex;

    World::World()
    {
        {
            std::lock_guard initLock(g_FlecsMutex);
            m_World = std::make_unique<flecs::world>();
            m_World->import <CoreEcsModule>();
        }
        RegisterModules();
    }

    World::World(
        World&& other) noexcept :
        m_World(std::move(other.m_World))
    {
    }

    World& World::operator=(
        World&& other) noexcept
    {
        if (this != &other)
        {
            if (m_World)
            {
                std::lock_guard initLock(g_FlecsMutex);
                m_World.reset();
            }
            m_World = std::move(other.m_World);
        }
        return *this;
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
        Entity entity(world.entity());
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
        double deltaTime)
    {
        m_World->progress(deltaTime);
    }

} // namespace Ame::Ecs