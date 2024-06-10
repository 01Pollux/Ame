#pragma once

#include <WorldNTree/WorldNTree.Mapper.hpp>

namespace Ame::Extensions
{
    template<typename TreeTy, typename ComponentTy>
    class WorldNTree
    {
    public:
        using tree_type           = TreeTy;
        using tree_container      = tree_type::Container;
        using node_data_type      = tree_type::data_type;
        using node_component_type = ComponentTy;

    public:
        WorldNTree(
            Ecs::Universe& universe) :
            m_Universe(universe),
            m_OnWorldChange(
                universe.OnWorldChange().ObjectSignal(),
                [this](auto& universe, auto& changeData)
                {
                    m_Tree.Reset();
                    CreateTransformObserver();
                })
        {
        }

    public:
        /// <summary>
        /// Runs the frustum culling algorithm on the tree.
        /// </summary>
        template<template<typename...> typename ContainerTy = std::vector>
        [[nodiscard]] auto FrustumCull(
            const Geometry::FrustumPlanes& planes,
            float                          tolerance = std::numeric_limits<tree_type::scalar_type>::epsilon()) const
        {
            auto activeWorld = m_Universe.get().GetActiveWorld();
            if (activeWorld) [[likely]]
            {
                return m_Tree.FrustumCulling(planes, tolerance) |
                       std::views::transform([&activeWorld](Ecs::Entity::Id id)
                                             { return activeWorld->GetEntityFromId(id); }) |
                       std::views::filter([](const Ecs::Entity& entity)
                                          { return static_cast<bool>(entity); }) |
                       std::ranges::to<ContainerTy>();
            }
            else
            {
                return ContainerTy{};
            }
        }

    private:
        /// <summary>
        /// Creates the transform observer.
        /// </summary>
        void CreateTransformObserver()
        {
            auto transformObserverCallback =
                [this](Ecs::Iterator& iter, const node_component_type* nodes)
            {
                for (size_t i : iter)
                {
                    Ecs::Entity entity(iter.entity(i));

                    if (iter.event() == flecs::OnSet)
                    {
                        constexpr bool has_specialization = !std::is_empty_v<Details::WorldNTreeMapper<ComponentTy, node_data_type>>;
                        if constexpr (has_specialization)
                        {
                            using Mapper = WorldNTreeMapper<node_component_type>;
                            m_Tree.Insert(entity.GetId(), Mapper::Map(entity, nodes[i]));
                        }
                        else
                        {
                            m_Tree.Insert(entity.GetId(), nodes[i]);
                        }
                    }
                    else
                    {
                        m_Tree.Remove(entity.GetId());
                    }
                }
            };

            m_WorldData.TransformObserver =
                world.CreateObserver<const node_component_type>()
                    .event(flecs::OnSet)
                    .event(flecs::OnRemove)
                    .iter(transformObserverCallback);
        }

    private:
        Ref<Ecs::Universe> m_Universe;

        Signals::OnWorldChange::Handle m_OnWorldChange;
        Ecs::UniqueObserver            m_TransformObserver;

        tree_container m_Tree;
    };

    //

    using WorldQuadTreePoint = WorldNTree<Geometry::QuadTreePoint, Ecs::Component::Transform>;
    using WorldOctTreePoint  = WorldNTree<Geometry::OctTreePoint, Ecs::Component::Transform>;
    using WorldHexTreePoint  = WorldNTree<Geometry::HexTreePoint, Ecs::Component::Transform>;

    using WorldQuadTreeBox = WorldNTree<Geometry::QuadTreeBoundingBox, Ecs::Component::AABB>;
    using WorldOctTreeBox  = WorldNTree<Geometry::OctTreeBoundingBox, Ecs::Component::AABB>;
    using WorldHexTreeBox  = WorldNTree<Geometry::HexTreeBoundingBox, Ecs::Component::AABB>;
} // namespace Ame::Extensions