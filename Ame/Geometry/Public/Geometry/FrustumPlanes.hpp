#pragma once

#include <vector>
#include <Math/Plane.hpp>
#include <Geometry/Frustum.hpp>

namespace Ame::Geometry
{
    struct FrustumPlanes
    {
    public:
        enum class Type : uint8_t
        {
            Near,
            Far,

            Right,
            Left,

            Top,
            Bottom,

            Count
        };
        using PlanesContainer = std::array<Math::Plane, static_cast<size_t>(Type::Count)>;

    public:
        PlanesContainer Planes;

    public:
        FrustumPlanes() = default;
        FrustumPlanes(
            const Frustum& frustum);

    public:
        operator std::span<Math::Plane>() noexcept
        {
            return Planes;
        }

        operator std::span<const Math::Plane>() const noexcept
        {
            return Planes;
        }

    public:
        Math::Plane& operator[](Type type) noexcept
        {
            return Planes[static_cast<size_t>(type)];
        }

        const Math::Plane& operator[](Type type) const noexcept
        {
            return Planes[static_cast<size_t>(type)];
        }
    };
} // namespace Ame::Geometry