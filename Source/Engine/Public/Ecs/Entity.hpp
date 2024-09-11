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
        Entity() = default;
        Entity(const flecs::entity& flecsEntity);

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
        void Reset(bool withChildren = true);

        /// <summary>
        /// Release all children of the entity.
        /// </summary>
        void RemoveAllChildren() const;

    public:
        [[nodiscard]] auto GetChildren(bool allowDisabled = true) const -> std::vector<Entity>;
        [[nodiscard]] auto GetParent() const -> Entity;
        [[nodiscard]] auto GetName() const -> const String&;

        void SetName(String name) const;

    public:
        /// <summary>
        /// Get the parent of the entity.
        /// </summary>
        void SetParent(const Entity& parent = {});

    private:
        flecs::entity m_Entity;
    };

    using UniqueEntity = Unique<Entity>;
} // namespace Ame::Ecs