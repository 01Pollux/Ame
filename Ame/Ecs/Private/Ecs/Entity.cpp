#include <Ecs/Entity.hpp>
#include <EcsUtil/Entity.hpp>

namespace Ame::Ecs
{
    Entity::Entity(
        flecs::entity FlecsEntity) :
        m_Entity(FlecsEntity)
    {
    }

    void Entity::Reset()
    {
        m_Entity.destruct();
        m_Entity = {};
    }

    Entity::operator bool() const
    {
        return m_Entity.is_alive();
    }

    //

    std::vector<Entity> Entity::GetChildren(
        bool AllowDisabled) const
    {
        std::vector<Entity> Children;
        if (!AllowDisabled)
        {
            m_Entity.children(
                [&Children](flecs::entity Child)
                {
                    Children.emplace_back(Child);
                });
        }
        else
        {
            auto Filter =
                m_Entity.world()
                    .filter_builder()
                    .term(flecs::ChildOf, m_Entity)
                    .term(flecs::Disabled)
                    .optional()
                    .build();

            Filter.iter(
                [&Children](flecs::iter& It)
                {
                    Children.reserve(Children.size() + It.count());
                    for (auto i = 0; i < It.count(); i++)
                    {
                        Children.emplace_back(It.entity(i));
                    }
                });
        }
        return Children;
    }

    //

    void Entity::SetParent(
        const Entity& Parent)
    {
        auto SafeName = EcsUtil::GetUniqueEntityName(m_Entity.world(), m_Entity.name(), Parent.GetFlecsEntity());
        m_Entity.set_name(SafeName.c_str());
        m_Entity.child_of(Parent.GetFlecsEntity());
    }

    //

    const flecs::entity& Entity::GetFlecsEntity() const
    {
        return m_Entity;
    }
} // namespace Ame::Ecs
