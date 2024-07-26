#include <Ecs/Entity.hpp>
#include <EcsUtil/Entity.hpp>

namespace Ame::Ecs
{
    Entity::Entity(
        const flecs::entity& flecsEntity) :
        m_Entity(flecsEntity)
    {
    }

    //

    auto Entity::GetId() const -> Id
    {
        return m_Entity.id();
    }

    const flecs::entity& Entity::GetFlecsEntity() const
    {
        return m_Entity;
    }

    const flecs::entity* Entity::operator->() const
    {
        return &m_Entity;
    }

    flecs::entity* Entity::operator->()
    {
        return &m_Entity;
    }

    const flecs::entity& Entity::operator*() const
    {
        return m_Entity;
    }

    flecs::entity& Entity::operator*()
    {
        return m_Entity;
    }

    bool Entity::operator==(
        const Entity& other) const
    {
        return m_Entity == other.m_Entity;
    }

    Entity::operator bool() const noexcept
    {
        return m_Entity && m_Entity.is_alive();
    }

    //

    void Entity::Reset(
        bool withChildren)
    {
        if (!withChildren)
        {
            // Move children to parent of parent.
            // and rename them to avoid name conflicts.
            auto OurParent = m_Entity.parent();
            for (auto& Child : GetChildren())
            {
                Child.SetParent(OurParent);
            }
        }

        m_Entity.destruct();
        m_Entity = {};
    }

    void Entity::RemoveAllChildren()
    {
        for (auto& Child : GetChildren())
        {
            Child.Reset();
        }
    }

    //

    std::vector<Entity> Entity::GetChildren(
        bool allowDisabled) const
    {
        std::vector<Entity> children;
        if (!allowDisabled)
        {
            m_Entity.children(
                [&children](const flecs::entity& Child)
                {
                    children.emplace_back(Child);
                });
        }
        else
        {
            auto filter =
                m_Entity.world()
                    .filter_builder()
                    .term(flecs::ChildOf, m_Entity)
                    .term(flecs::Disabled)
                    .optional()
                    .build();

            filter.iter(
                [&children](flecs::iter& It)
                {
                    children.reserve(children.size() + It.count());
                    for (auto i = 0; i < It.count(); i++)
                    {
                        children.emplace_back(It.entity(i));
                    }
                });
        }
        return children;
    }

    //

    void Entity::SetParent(
        const Entity& parent)
    {
        auto SafeName = EcsUtil::GetUniqueEntityName(m_Entity.world(), m_Entity.name(), parent.GetFlecsEntity());
        m_Entity.set_name(SafeName.c_str());
        m_Entity.child_of(parent.GetFlecsEntity());
    }
} // namespace Ame::Ecs
