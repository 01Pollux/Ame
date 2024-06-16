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
        // Workaround to get this world from flecs world.
        struct ThisWorld
        {
            World* Self;
        };

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
        [[nodiscard]] flecs::world& Get() noexcept
        {
            return *m_World;
        }

        [[nodiscard]] const flecs::world& Get() const noexcept
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
            m_World(world)
        {
        }

        WorldRef(
            flecs::world world) :
            m_World(*world.get<World::ThisWorld>()->Self)
        {
        }

    public:
        [[nodiscard]] auto operator->() const
        {
            return &m_World.get();
        }

        [[nodiscard]] auto& operator*() const
        {
            return m_World.get();
        }

    private:
        Ref<World> m_World;
    };

    //

    class AsyncWorld
    {
    public:
        AsyncWorld(
            World& world) :
            m_World(world.Get().async_stage())
        {
        }

        AsyncWorld(
            flecs::world world) :
            m_World(std::move(world))
        {
        }

        void Merge()
        {
            m_World.merge();
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

    private:
        flecs::world m_World;
    };
} // namespace Ame::Ecs