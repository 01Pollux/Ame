#include <Ecs/World.hpp>
#include <EcsUtil/Entity.hpp>

#include <Log/Wrapper.hpp>

#include <mutex>

namespace Ame::Ecs
{
    /// <summary>
    /// The mutex for initializing flecs world.
    /// It is used to prevent multiple threads from initializing the world at the same time.
    /// </summary>
    static std::mutex g_FlecsMutex;

    struct WorldName
    {
        String Name;
    };

    World::World(
        const String& name)
    {
        {
            std::lock_guard initLock(g_FlecsMutex);
            m_World = std::make_unique<flecs::world>();

            m_World->component<WorldName>();
            m_World->emplace<WorldName>(name);
        }
        RegisterModules();
    }

    World::World(
        flecs::world& world)
    {
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
        if (m_World && m_World->m_owned)
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
        auto entity = world.entity();
        if (parent)
        {
            entity.child_of(parent.GetFlecsEntity());
        }
        entity.set_name(name.data());
        return Entity(entity);
    }

    //

    Entity World::CreateEntity(
        StringView    name,
        const Entity& parent)
    {
        return CreateEntityFromWorld(*m_World, name, parent);
    }

    Entity World::GetEntityById(
        const Entity::Id id) const
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
        const Entity::Id id) const
    {
        return Entity(m_World.entity(id));
    }

    //

    String World::GetUniqueEntityName(
        const char*   name,
        const Entity& parent) const
    {
        return EcsUtil::GetUniqueEntityName(*m_World, name, parent.GetFlecsEntity());
    }

    //

    String WorldRef::GetUniqueEntityName(
        const char*   name,
        const Entity& parent) const
    {
        return EcsUtil::GetUniqueEntityName(m_World, name, parent.GetFlecsEntity());
    }

    //

    void World::Progress(
        double deltaTime)
    {
        m_World->progress(deltaTime);
    }

} // namespace Ame::Ecs