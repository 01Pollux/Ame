#pragma once

#include <Core/Subsystem.hpp>

#include <Ecs/Subsystem/Universe.hpp>

#include <WorldNTree/WorldNTree.hpp>

namespace Ame::Extensions
{
    template<typename TreeTy, typename ComponentTy>
    struct WorldNTreeSubsystem : SingleSubsystem<
                                     WorldNTree<TreeTy, ComponentTy>,
                                     Dependency<Ecs::UniverseSubsystem>>,
                                 kgr::final
    {
    };

    template<typename TreeTy, typename ComponentTy>
    auto service_map(const WorldNTree<TreeTy, ComponentTy>&) -> WorldNTreeSubsystem<TreeTy, ComponentTy>;

    //

    using WorldQuadTreePointSubsystem = WorldNTreeSubsystem<Geometry::QuadTreePoint, Ecs::Component::Transform>;
    using WorldOctTreePointSubsystem  = WorldNTreeSubsystem<Geometry::OctTreePoint, Ecs::Component::Transform>;
    using WorldHexTreePointSubsystem  = WorldNTreeSubsystem<Geometry::HexTreePoint, Ecs::Component::Transform>;

    using WorldQuadTreeBoxSubsystem = WorldNTreeSubsystem<Geometry::QuadTreeBoundingBox, Ecs::Component::AABB>;
    using WorldOctTreeBoxSubsystem  = WorldNTreeSubsystem<Geometry::OctTreeBoundingBox, Ecs::Component::AABB>;
    using WorldHexTreeBoxSubsystem  = WorldNTreeSubsystem<Geometry::HexTreeBoundingBox, Ecs::Component::AABB>;
} // namespace Ame::Extensions
