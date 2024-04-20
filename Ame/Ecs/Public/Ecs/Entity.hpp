#pragma once

#include <Ecs/Core.hpp>

namespace Ame::Ecs
{
    class Entity
    {
        friend class World;

    public:
        static const Entity Null;

        Entity() = default;
        Entity(
            flecs::entity FlecsEntity);

    public:
        operator bool() const;

    public:
        /// <summary>
        /// Get the children of the entity.
        /// </summary>
        [[nodiscard]] std::vector<Entity> GetChildren(
            bool AllowDisabled = true) const;

    public:
        /// <summary>
        /// Get the parent of the entity.
        /// </summary>
        void SetParent(
            const Entity& Parent = Entity::Null);

    private:
        /// <summary>
        /// Get the flecs entity
        /// </summary>
        const flecs::entity& GetFlecsEntity() const;

    private:
        flecs::entity m_Entity;
    };

    inline const Entity Entity::Null = Entity{};
} // namespace Ame::Ecs