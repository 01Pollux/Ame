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
            flecs::entity flecsEntity);

    public:
        operator bool() const;

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

    public:
        /// <summary>
        /// Get the parent of the entity.
        /// </summary>
        void SetParent(
            const Entity& parent = Entity::Null);

    public:
        template<typename Ty, typename... ArgsTy>
        void AddComponent(
            ArgsTy&&... args)
        {
            m_Entity.emplace<Ty>(std::forward<ArgsTy>(args)...);
        }

        template<typename Ty>
        void AddComponent(
            Ty&& data)
        {
            m_Entity.emplace<Ty>(std::forward<Ty>(data));
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
        void MarkModified()
        {
            m_Entity.modified<Ty>();
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