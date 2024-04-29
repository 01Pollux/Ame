#pragma once

#include <Ecs/Entity.hpp>
#include <Ecs/Rule.hpp>

namespace Ame::Ecs
{
    class World
    {
    public:
        World(
            const StringU8& Name);

        World(const World&)            = delete;
        World& operator=(const World&) = delete;

        World(World&&) noexcept;
        World& operator=(World&&) noexcept;

        ~World();

    public:
        /// <summary>
        /// Create a new entity in the world.
        /// If the entity's name is already in use, Name will be modified to be unique using 'CreateUniqueEntityName'.
        /// </summary>
        Entity CreateEntity(
            const StringU8& Name,
            const Entity&   Parent = Entity::Null);

        /// <summary>
        /// Destroy an entity in the world.
        /// </summary>
        void DestroyEntity(
            const Entity& EcsEntity,
            bool          WithChildren = true);

        /// <summary>
        /// Get an entities by its name.
        /// </summary>
        [[nodiscard]] std::vector<Entity> GetEntities(
            const StringU8& Name);

    public:
        /// <summary>
        /// Get unique entity name that is not already in use.
        /// </summary>
        [[nodiscard]] StringU8 GetUniqueEntityName(
            const char*   Name,
            const Entity& Parent = Entity::Null) const;

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
        /// Progress the world.
        /// </summary>
        void Progress(
            double DeltaTime);

    private:
        /// <summary>
        /// Register all components in the world.
        /// </summary>
        void RegisterModules();

    private:
        UPtr<flecs::world> m_World;
    };
} // namespace Ame::Ecs