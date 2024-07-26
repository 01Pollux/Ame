#pragma once

#include <vector>
#include <Ecs/Core.hpp>

namespace Ame::Ecs
{
    using EntityId = flecs::entity_t;
    class Entity
    {
        friend class World;

    public:
        static const Entity c_Null;

    public:
        Entity() = default;
        Entity(
            const flecs::entity& flecsEntity);

    public:
        /// <summary>
        /// Get the underlying flecs entity id
        /// </summary>
        [[nodiscard]] EntityId GetId() const;

        /// <summary>
        /// Get the underlying flecs entity
        /// </summary>
        [[nodiscard]] const flecs::entity& GetFlecsEntity() const;

        [[nodiscard]] const flecs::entity* operator->() const;
        [[nodiscard]] flecs::entity*       operator->();
        [[nodiscard]] const flecs::entity& operator*() const;
        [[nodiscard]] flecs::entity&       operator*();

        [[nodiscard]] bool operator==(const Entity& other) const;

        [[nodiscard]] explicit operator bool() const noexcept;

    public:
        /// <summary>
        /// Release the entity.
        /// </summary>
        void Reset(
            bool withChildren = false);

        /// <summary>
        /// Release all children of the entity.
        /// </summary>
        void RemoveAllChildren();

    public:
        /// <summary>
        /// Get the children of the entity.
        /// </summary>
        [[nodiscard]] std::vector<Entity> GetChildren(
            bool allowDisabled = true) const;

        void                        SetName(String name) const;
        [[nodiscard]] const String& GetName() const;

    public:
        /// <summary>
        /// Get the parent of the entity.
        /// </summary>
        void SetParent(
            const Entity& parent = Entity::c_Null);

    private:
        flecs::entity m_Entity;
    };

    inline const Entity Entity::c_Null = Entity{};

    using UniqueEntity = Unique<Entity>;
} // namespace Ame::Ecs