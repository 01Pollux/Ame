#pragma once

#include <Ecs/Entity.hpp>

#include <Ecs/Filter.hpp>
#include <Ecs/Query.hpp>
#include <Ecs/Rule.hpp>
#include <Ecs/System.hpp>
#include <Ecs/Observer.hpp>

namespace Ame::Ecs
{
    class World
    {
    public:
        World(
            const String& name);
        World(
            flecs::world& world);

        World(
            const World&) = delete;
        World(
            World&&) noexcept;

        World& operator=(
            const World&) = delete;
        World& operator=(
            World&&) noexcept;

        ~World();

    public:
        /// <summary>
        /// Create a new entity in the world.
        /// If the entity's name is already in use, Name will be modified to be unique using 'CreateUniqueEntityName'.
        /// </summary>
        Entity CreateEntity(
            StringView    name,
            const Entity& parent = Entity::c_Null);

        /// <summary>
        /// Get entity from its id.
        /// </summary>
        [[nodiscard]] Entity GetEntityFromId(
            const Entity::Id id) const;

    public:
        /// <summary>
        /// Get unique entity name that is not already in use.
        /// </summary>
        [[nodiscard]] String GetUniqueEntityName(
            const char*   name,
            const Entity& parent = Entity::c_Null) const;

    public:
        /// <summary>
        /// Create a new filter in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] FilterBuilder<ArgsTy...> CreateFilter()
        {
            return m_World->filter_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new query in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] QueryBuilder<ArgsTy...> CreateQuery()
        {
            return m_World->query_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new rule in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] RuleBuilder<ArgsTy...> CreateRule()
        {
            return m_World->rule_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new system in the world for processing entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] SystemBuilder<ArgsTy...> CreateSystem()
        {
            return m_World->system<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new observer in the world for tracking entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] ObserverBuilder<ArgsTy...> CreateObserver()
        {
            return m_World->observer<ArgsTy...>();
        }

    public:
        [[nodiscard]] auto operator->() const noexcept
        {
            return m_World.get();
        }

        [[nodiscard]] auto& operator*()
        {
            return *m_World;
        }

    public:
        /// <summary>
        /// Progress the world.
        /// </summary>
        void Progress(
            double deltaTime);

    public:
        /// <summary>
        /// Register a new module in the world.
        /// </summary>
        template<typename Ty>
        void ImportModule()
        {
            m_World->import <Ty>();
        }

    private:
        /// <summary>
        /// Register all components in the world.
        /// </summary>
        void RegisterModules();

    private:
        UPtr<flecs::world> m_World;
    };

    //

    class WorldRef
    {
    public:
        WorldRef(
            World& world) :
            m_World(world->get_world())
        {
        }

        WorldRef(
            flecs::world world) :
            m_World(std::move(world))
        {
        }

    public:
        /// <summary>
        /// Create a new entity in the world.
        /// If the entity's name is already in use, Name will be modified to be unique using 'CreateUniqueEntityName'.
        /// </summary>
        Entity CreateEntity(
            StringView    name,
            const Entity& parent = Entity::c_Null);

        /// <summary>
        /// Get entity from its id.
        /// </summary>
        [[nodiscard]] Entity GetEntityFromId(
            const Entity::Id id) const;

    public:
        /// <summary>
        /// Get unique entity name that is not already in use.
        /// </summary>
        [[nodiscard]] String GetUniqueEntityName(
            const char*   name,
            const Entity& parent = Entity::c_Null) const;

    public:
        /// <summary>
        /// Create a new filter in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] FilterBuilder<ArgsTy...> CreateFilter()
        {
            return m_World.filter_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new query in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] QueryBuilder<ArgsTy...> CreateQuery()
        {
            return m_World.query_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new rule in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] RuleBuilder<ArgsTy...> CreateRule()
        {
            return m_World.rule_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new system in the world for processing entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] SystemBuilder<ArgsTy...> CreateSystem()
        {
            return m_World.system<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new observer in the world for tracking entities.
        /// </summary>
        template<typename... ArgsTy>
        [[nodiscard]] ObserverBuilder<ArgsTy...> CreateObserver()
        {
            return m_World.observer<ArgsTy...>();
        }

    public:
        [[nodiscard]] auto operator->()
        {
            return &m_World;
        }

        [[nodiscard]] auto& operator*()
        {
            return m_World;
        }

    public:
        /// <summary>
        /// Register a new module in the world.
        /// </summary>
        template<typename Ty>
        void ImportModule()
        {
            m_World.import <Ty>();
        }

    protected:
        flecs::world m_World;
    };

    //

    class AsyncWorld : public WorldRef
    {
    public:
        AsyncWorld(
            World& world) :
            WorldRef(world->async_stage())
        {
        }

        AsyncWorld(
            flecs::world world) :
            WorldRef(world.async_stage())
        {
        }

        void Merge()
        {
            m_World.merge();
        }
    };

    //

    class ScopedDeferredWorld : public WorldRef
    {
    public:
        ScopedDeferredWorld(
            World& world) :
            WorldRef(world)
        {
            m_World.defer_begin();
        }

        ScopedDeferredWorld(
            flecs::world world) :
            WorldRef(std::move(world))
        {
            m_World.defer_begin();
        }

        ScopedDeferredWorld(const ScopedDeferredWorld&) = delete;
        ScopedDeferredWorld(ScopedDeferredWorld&&)      = delete;

        ScopedDeferredWorld& operator=(const ScopedDeferredWorld&) = delete;
        ScopedDeferredWorld& operator=(ScopedDeferredWorld&&)      = delete;

        ~ScopedDeferredWorld()
        {
            m_World.defer_end();
        }
    };

    //

    class ScopedDeferredSuspensionWorld : public WorldRef
    {
    public:
        ScopedDeferredSuspensionWorld(
            World& world) :
            WorldRef(world)
        {
            m_World.defer_suspend();
        }

        ScopedDeferredSuspensionWorld(
            flecs::world world) :
            WorldRef(std::move(world))
        {
            m_World.defer_suspend();
        }

        ScopedDeferredSuspensionWorld(const ScopedDeferredSuspensionWorld&) = delete;
        ScopedDeferredSuspensionWorld(ScopedDeferredSuspensionWorld&&)      = delete;

        ScopedDeferredSuspensionWorld& operator=(const ScopedDeferredSuspensionWorld&) = delete;
        ScopedDeferredSuspensionWorld& operator=(ScopedDeferredSuspensionWorld&&)      = delete;

        ~ScopedDeferredSuspensionWorld()
        {
            m_World.defer_resume();
        }
    };

    //

    class ScopedAsyncWorld : public AsyncWorld
    {
    public:
        ScopedAsyncWorld(
            World& world) :
            AsyncWorld(world)
        {
            m_World.defer_begin();
        }

        ScopedAsyncWorld(
            flecs::world world) :
            AsyncWorld(std::move(world))
        {
            m_World.defer_begin();
        }

        ScopedAsyncWorld(const ScopedAsyncWorld&) = delete;
        ScopedAsyncWorld(ScopedAsyncWorld&&)      = delete;

        ScopedAsyncWorld& operator=(const ScopedAsyncWorld&) = delete;
        ScopedAsyncWorld& operator=(ScopedAsyncWorld&&)      = delete;

        ~ScopedAsyncWorld()
        {
            m_World.defer_end();
        }
    };
} // namespace Ame::Ecs