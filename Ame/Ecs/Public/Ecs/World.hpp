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
        /// <summary>
        /// Progress the world.
        /// </summary>
        void Progress(
            double deltaTime);

    private:
        /// <summary>
        /// Register all components in the world.
        /// </summary>
        void RegisterModules();

    private:
        UPtr<flecs::world> m_World;
    };
} // namespace Ame::Ecs