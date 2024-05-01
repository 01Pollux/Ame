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

    public:
        template<typename Ty, typename... ArgsTy>
        void AddComponent(
            ArgsTy&&... Args)
        {
            m_Entity.emplace<Ty>(std::forward<ArgsTy>(Args)...);
        }

        template<typename Ty>
        void RemoveComponent()
        {
            m_Entity.remove<Ty>();
        }

        template<typename Ty>
        [[nodiscard]] bool HasComponent() const
        {
            return m_Entity.has<Ty>();
        }

        template<typename Ty>
        [[nodiscard]] const Ty& GetComponent() const
        {
            return *m_Entity.get<Ty>();
        }

        template<typename Ty>
        [[nodiscard]] Ty& GetComponentMut()
        {
            return *m_Entity.get_mut<Ty>();
        }

        template<typename Ty>
        [[nodiscard]] std::optional<const Ty*> TryGetComponent() const
        {
            return m_Entity.get<Ty>();
        }

    private:
        /// <summary>
        /// Get the flecs entity
        /// </summary>
        const flecs::entity& GetFlecsEntity() const;

    private:
        flecs::entity m_Entity;
    };

    inline const Entity Entity::Null = Entity{};

    using UniqueEntity = Unique<Entity>;
} // namespace Ame::Ecs