#pragma once

#include <WorldNTree/WorldNTree.Mapper.hpp>

namespace Ame::Extensions
{
    struct WorldNTreeDesc
    {
        uint32_t MaxDepth        = 6;
        float    BoxExpandFactor = 1.5f;
    };

    template<typename TreeTy, typename ComponentTy>
    class WorldNTree
    {
    public:
        using tree_type           = TreeTy;
        using node_data_type      = tree_type::data_type;
        using node_component_type = ComponentTy;

    private:
        /// <summary>
        /// Helper class to access entity data by id.
        /// </summary>
        class EntityDataAccessor
        {
        public:
            using value_type = node_data_type;

        public:
            EntityDataAccessor(
                Ecs::World& world) :
                m_World(world)
            {
            }

        public:
            [[nodiscard]] size_t size() const
            {
                if (!m_Size)
                {
                    m_Size = m_World.GetEntityCount();
                }
                return m_Size;
            }

            [[nodiscard]] node_data_type operator[](size_t entityId) const
            {
                using Mapper = WorldNTreeMapper<node_component_type, node_data_type>;

                auto entity = m_World.GetEntityFromId(entityId);
                return Mapper::Map(entity, entity.GetComponent<node_component_type>());
            }

        private:
            Ecs::World&    m_World;
            mutable size_t m_Size = 0;
        };

    public:
        WorldNTree(
            Ecs::Universe& universe,
            WorldNTreeDesc desc = {}) :
            m_Universe(universe),
            m_Desc(desc),
            m_OnWorldChange(
                universe.OnWorldChange().ObjectSignal(),
                [this](auto& universe, auto& changeData)
                {
                    HookToWorld(changeData.NewWorld);
                })
        {
            HookToWorld(m_Universe.get().GetActiveWorld());
        }

    public:
        /// <summary>
        /// Runs the frustum culling algorithm on the tree.
        /// </summary>
        template<std::ranges::range ContainerTy>
        void FrustumCull(
            ContainerTy&                   entities,
            const Geometry::FrustumPlanes& frustum,
            float                          tolerance = std::numeric_limits<tree_type::scalar_type>::epsilon()) const
        {
            auto activeWorld = m_Universe.get().GetActiveWorld();
            if (activeWorld) [[likely]]
            {
                EntityDataAccessor accessor(*activeWorld);

                auto view = m_Tree.FrustumCulling(frustum.GetPlanes(), tolerance, accessor) |
                            std::views::transform([&activeWorld](Ecs::Entity::Id id)
                                                  { return activeWorld->GetEntityFromId(id); }) |
                            std::views::filter([](const Ecs::Entity& entity)
                                               { return static_cast<bool>(entity); });

                auto allNodes = m_Tree.GetNodes();

                std::ranges::copy(
                    std::move(view),
                    std::inserter(entities, entities.end()));
            }
        }

    private:
        /// <summary>
        /// Install hooks to the world.
        /// </summary>
        void HookToWorld(
            Ecs::World* world)
        {
            m_Tree.Reset();
            tree_type::Tree::Create(m_Tree, {}, m_Desc.MaxDepth);

            m_TransformObserver.Reset();
            if (world)
            {
                CreateTransformObserver(*world);
            }
        }

        /// <summary>
        /// Creates the transform observer.
        /// </summary>
        void CreateTransformObserver(
            Ecs::World& world)
        {
            auto transformObserverCallback =
                [this](Ecs::Iterator&             iter,
                       const node_component_type* nodes)
            {
                auto& world = *m_Universe.get().GetActiveWorld();

                std::span<const node_component_type> allNodes(nodes, iter.count());
                TryFitWorld(world, allNodes);

                for (size_t i : iter)
                {
                    Ecs::Entity::Id entityId(iter.entity(i));
                    if (iter.event() == flecs::OnSet)
                    {
                        AddEntityToWorld(world, entityId, nodes[i]);
                    }
                    else
                    {
                        RemoveEntityFromWorld(entityId);
                    }
                }
            };

            m_TransformObserver =
                world.CreateObserver<const node_component_type>()
                    .event(flecs::OnSet)
                    .event(flecs::OnRemove)
                    .yield_existing()
                    .iter(transformObserverCallback);
        }

        /// <summary>
        /// Accumulates the bounds of the nodes.
        /// </summary>
        [[nodiscard]] Geometry::AABBMinMax GetBounds(
            std::span<const node_component_type> nodes) const
        {
            Geometry::AABBMinMax bounds;
            for (const auto& node : nodes)
            {
                bounds.Accumulate(node);
            }
            return bounds;
        }

        /// <summary>
        /// Checks if the world can fit into the tree.
        /// </summary>
        /// <returns></returns>
        [[nodiscard]] bool CanFitWorld(
            const Geometry::AABBMinMax& bounds) const
        {
            return m_Tree.GetBox().Contains(bounds.ToAABB());
        }

        /// <summary>
        /// Tries to fit the world into the tree.
        /// </summary>
        void TryFitWorld(
            Ecs::World&                          world,
            std::span<const node_component_type> nodes)
        {
            auto bounds = GetBounds(nodes);
            if (CanFitWorld(bounds))
            {
                return;
            }

            auto allEntities = m_Tree.CollectAllIdInBFS();
            m_Tree.Reset();

            bounds.Min *= m_Desc.BoxExpandFactor;
            bounds.Max *= m_Desc.BoxExpandFactor;

            tree_type::Tree::Create(m_Tree, {}, m_Desc.MaxDepth, bounds.ToAABB());
            for (auto entityId : allEntities)
            {
                AddEntityToWorld(world, static_cast<Ecs::Entity::Id>(entityId));
            }
        }

        /// <summary>
        /// Adds the entity to the world.
        /// </summary>
        void AddEntityToWorld(
            Ecs::World&     world,
            Ecs::Entity::Id entityId)
        {
            using Mapper = WorldNTreeMapper<node_component_type, node_data_type>;

            auto entity = world.GetEntityFromId(entityId);
            m_Tree.Insert(entityId, Mapper::Map(entity, entity.GetComponent<node_component_type>()));
        }

        /// <summary>
        /// Adds the entity to the world.
        /// </summary>
        void AddEntityToWorld(
            Ecs::World&                world,
            Ecs::Entity::Id            entityId,
            const node_component_type& node)
        {
            using Mapper = WorldNTreeMapper<node_component_type, node_data_type>;

            auto entity = world.GetEntityFromId(entityId);
            m_Tree.Insert(entityId, Mapper::Map(entity, node));
        }

        /// <summary>
        /// Removes the entity to the world.
        /// </summary>
        void RemoveEntityFromWorld(
            Ecs::Entity::Id entityId)
        {
            m_Tree.EraseId<false>(entityId);
        }

    private:
        Ref<Ecs::Universe> m_Universe;
        WorldNTreeDesc     m_Desc;

        Signals::OnWorldChange::Handle m_OnWorldChange;
        Ecs::UniqueObserver            m_TransformObserver;

        tree_type::Tree m_Tree;
    };

    //

    using WorldQuadTreePoint = WorldNTree<Geometry::QuadTreePoint, Ecs::Component::Transform>;
    using WorldOctTreePoint  = WorldNTree<Geometry::OctTreePoint, Ecs::Component::Transform>;
    using WorldHexTreePoint  = WorldNTree<Geometry::HexTreePoint, Ecs::Component::Transform>;

    using WorldQuadTreeBox = WorldNTree<Geometry::QuadTreeBoundingBox, Ecs::Component::AABB>;
    using WorldOctTreeBox  = WorldNTree<Geometry::OctTreeBoundingBox, Ecs::Component::AABB>;
    using WorldHexTreeBox  = WorldNTree<Geometry::HexTreeBoundingBox, Ecs::Component::AABB>;
} // namespace Ame::Extensions