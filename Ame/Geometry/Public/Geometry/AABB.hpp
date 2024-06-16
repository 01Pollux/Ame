#pragma once

#include <Geometry/Common.hpp>

namespace Ame::Geometry
{
    struct AABB : Math::Util::BoundingBox
    {
    public:
        using BoundingBox::BoundingBox;

        AABB() :
            BoundingBox{ Math::Vector3::Constants::Zero, Math::Vector3::Constants::Zero }
        {
        }

    public:
        [[nodiscard]] static AABB FromMinMax(
            const Math::Vector3& min,
            const Math::Vector3& max) noexcept
        {
            return AABB((min + max) * 0.5f, (max - min) * 0.5f);
        }

        [[nodiscard]] Math::Vector3 Min() const noexcept
        {
            return Math::Vector3(Center) - Extents;
        }

        [[nodiscard]] Math::Vector3 Max() const noexcept
        {
            return Math::Vector3(Center) + Extents;
        }

        [[nodiscard]] Math::Vector3 Size() const noexcept
        {
            return Math::Vector3(Extents) * 2.0f;
        }

    public:
        using BoundingBox::Transform;

        [[nodiscard]] AABB Transform(
            const Math::Matrix4x4& transform) const noexcept
        {
            auto copy = *this;
            Transform(copy, transform);
            return copy;
        }
    };

    struct AABBMinMax
    {
        Math::Vector3 Min = Math::Vector3::Constants::Max;
        Math::Vector3 Max = Math::Vector3::Constants::Min;

        AABBMinMax() = default;
        AABBMinMax(const Math::Vector3& min, const Math::Vector3& max) :
            Min(min),
            Max(max)
        {
        }
        AABBMinMax(const AABB& aabb) :
            Min(aabb.Min()),
            Max(aabb.Max())
        {
        }

        [[nodiscard]] AABB ToAABB() const noexcept
        {
            return AABB::FromMinMax(Min, Max);
        }

        [[nodiscard]] Math::Vector3 Center() const noexcept
        {
            return (Min + Max) * 0.5f;
        }

        [[nodiscard]] Math::Vector3 Extents() const noexcept
        {
            return (Max - Min) * 0.5f;
        }

        [[nodiscard]] Math::Vector3 Size() const noexcept
        {
            return (Max - Min);
        }

        void Accumulate(
            const Math::Vector3& point) noexcept
        {
            Min = point.min(Min);
            Max = point.max(Max);
        }

        void Accumulate(
            const AABB& aabb) noexcept
        {
            Min = aabb.Min().min(Min);
            Max = aabb.Max().max(Max);
        }

        void Accumulate(
            const AABBMinMax& aabb) noexcept
        {
            Min = aabb.Min.min(Min);
            Max = aabb.Max.max(Max);
        }

        [[nodiscard]] AABBMinMax Transform(
            const Math::Matrix4x4& transform) const noexcept
        {
            return {
                transform.DoTransformCoord(Min),
                transform.DoTransformCoord(Max)
            };
        }
    };
} // namespace Ame::Geometry