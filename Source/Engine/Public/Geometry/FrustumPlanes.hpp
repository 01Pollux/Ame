#pragma once

#include <vector>
#include <span>
#include <Math/Plane.hpp>
#include <Geometry/Frustum.hpp>

namespace Ame::Geometry
{
    struct AME_ENGINE_API FrustumPlanes
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
        static constexpr uint32_t PlanesCount = static_cast<uint32_t>(Type::Count);
        using PlanesContainer                 = std::array<Math::Plane, PlanesCount>;

    public:
        PlanesContainer Planes;

    public:
        constexpr FrustumPlanes() = default;
        FrustumPlanes(const Frustum& frustum);

    public:
        [[nodiscard]] std::span<Math::Plane> GetPlanes() noexcept
        {
            return Planes;
        }

        [[nodiscard]] std::span<const Math::Plane> GetPlanes() const noexcept
        {
            return Planes;
        }

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
        [[nodiscard]] constexpr Math::Plane& operator[](Type type) noexcept
        {
            return Planes[static_cast<size_t>(type)];
        }

        [[nodiscard]] constexpr const Math::Plane& operator[](Type type) const noexcept
        {
            return Planes[static_cast<size_t>(type)];
        }
    };
} // namespace Ame::Geometry