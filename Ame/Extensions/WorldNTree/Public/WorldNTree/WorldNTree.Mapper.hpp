#pragma once

#include <Core/Coroutine.hpp>
#include <Geometry/NTree.hpp>
#include <Geometry/FrustumPlanes.hpp>

#include <WorldNTree/Core.hpp>

namespace Ame::Extensions
{
    /// <summary>
    /// Must implement the following:
    /// - [[nodiscard]] static const DataTy& Map(const Ecs::Entity&, const ComponentTy&)
    /// </summary>
    template<typename ComponentTy, typename DataTy>
    struct WorldNTreeMapper;

    template<>
    struct WorldNTreeMapper<Ecs::Component::Transform, Math::Vector3>
    {
        [[nodiscard]] static const Math::Vector3& Map(const Ecs::Entity&, const Ecs::Component::Transform& transform)
        {
            return transform.GetPosition();
        }
    };

    template<>
    struct WorldNTreeMapper<Ecs::Component::AABB, Geometry::AABB>
    {
        [[nodiscard]] static Geometry::AABB Map(const Ecs::Entity&, const Ecs::Component::AABB& box)
        {
            return box.ToAABB();
        }
    };
} // namespace Ame::Extensions