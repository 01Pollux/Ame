#pragma once

#include <flecs.h>
#include <Core/String.hpp>

namespace Ame::Ecs
{
    template<typename Ty>
    struct Unique
    {
    public:
        Unique() = default;
        Unique(
            const Ty& entity) :
            m_Entity(entity)
        {
        }

        Unique(
            const Unique&) = delete;
        Unique(
            Unique&& other) noexcept :
            m_Entity(std::exchange(other.m_Entity, Ty{}))
        {
        }

        Unique& operator=(
            const Unique&) = delete;
        Unique& operator=(
            Unique&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                Reset();
                m_Entity = std::exchange(other.m_Entity, Ty{});
            }

            return *this;
        }

        ~Unique()
        {
            Reset();
        }

    public:
        [[nodiscard]] explicit operator bool() const noexcept
        {
            return IsValid();
        }

        /// <summary>
        /// Check if the entity is valid.
        /// </summary>
        [[nodiscard]] bool IsValid() const
        {
            if (!m_Entity)
            {
                return false;
            }
            if constexpr (requires { m_Entity.is_alive(); })
                return m_Entity.is_alive();
            else if constexpr (requires { m_Entity.is_valid(); })
                return m_Entity.is_valid();
            else
                return true;
        }

        [[nodiscard]] auto operator->() const
        {
            return &m_Entity;
        }

        [[nodiscard]] auto operator->()
        {
            return &m_Entity;
        }

        [[nodiscard]] operator Ty() const
        {
            return Get();
        }

        /// <summary>
        /// Get the entity.
        /// </summary>
        [[nodiscard]] const Ty& Get() const
        {
            return m_Entity;
        }

        /// <summary>
        /// Discard the entity without releasing it.
        /// </summary>
        void Discard()
        {
            m_Entity = Ty{};
        }

        /// <summary>
        /// Release the entity if it is valid.
        /// </summary>
        void Reset()
        {
            if (IsValid())
            {
                if constexpr (requires { m_Entity.destruct(); })
                {
                    m_Entity.destruct();
                }
                else if constexpr (requires { m_Entity.Reset(); })
                {
                    m_Entity.Reset();
                }
                m_Entity = Ty{};
            }
        }

    private:
        Ty m_Entity{};
    };
} // namespace Ame::Ecs